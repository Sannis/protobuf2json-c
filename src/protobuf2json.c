/*
 * Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#include <assert.h>
#include <string.h>

#include "protobuf2json.h"

/* For compatibility with old Jansson from repositories */
#include "jansson_compat.h"

/* === Protobuf -> JSON === Private === */

static size_t protobuf2json_value_size_by_type(ProtobufCType type) {
  switch (type) {
    case PROTOBUF_C_TYPE_INT32:
    case PROTOBUF_C_TYPE_SINT32:
    case PROTOBUF_C_TYPE_SFIXED32:
    case PROTOBUF_C_TYPE_UINT32:
    case PROTOBUF_C_TYPE_FIXED32:
    case PROTOBUF_C_TYPE_FLOAT:
      return 4;
    case PROTOBUF_C_TYPE_ENUM:
      return 4;
    case PROTOBUF_C_TYPE_INT64:
    case PROTOBUF_C_TYPE_SINT64:
    case PROTOBUF_C_TYPE_SFIXED64:
    case PROTOBUF_C_TYPE_UINT64:
    case PROTOBUF_C_TYPE_FIXED64:
    case PROTOBUF_C_TYPE_DOUBLE:
      return 8;
    case PROTOBUF_C_TYPE_BOOL:
      return sizeof(protobuf_c_boolean);
    case PROTOBUF_C_TYPE_STRING:
      return sizeof(char *);
    case PROTOBUF_C_TYPE_MESSAGE:
      return sizeof(ProtobufCMessage *);
    case PROTOBUF_C_TYPE_BYTES:
      return sizeof(ProtobufCBinaryData);
    /* case PROTOBUF_C_TYPE_GROUP: - NOT SUPPORTED */
    default:
      assert(0);
      return 0;
  }
}

static json_t* protobuf2json_process_message(const ProtobufCMessage *protobuf_message);

static json_t* protobuf2json_process_field(const ProtobufCFieldDescriptor *field_descriptor, const void *protobuf_value) {
  switch (field_descriptor->type) {
    case PROTOBUF_C_TYPE_INT32:
    case PROTOBUF_C_TYPE_SINT32:
    case PROTOBUF_C_TYPE_SFIXED32:
      return json_integer(*(int32_t *)protobuf_value);
    case PROTOBUF_C_TYPE_UINT32:
    case PROTOBUF_C_TYPE_FIXED32:
      return json_integer(*(uint32_t *)protobuf_value);
    case PROTOBUF_C_TYPE_INT64:
    case PROTOBUF_C_TYPE_SINT64:
    case PROTOBUF_C_TYPE_SFIXED64:
      return json_integer(*(int64_t *)protobuf_value);
    case PROTOBUF_C_TYPE_UINT64:
    case PROTOBUF_C_TYPE_FIXED64:
      return json_integer(*(uint64_t *)protobuf_value);
    case PROTOBUF_C_TYPE_DOUBLE:
      return json_real(*(double *)protobuf_value);
    case PROTOBUF_C_TYPE_FLOAT:
      return json_real(*(float *)protobuf_value);
    case PROTOBUF_C_TYPE_BOOL:
      return json_boolean(*(protobuf_c_boolean *)protobuf_value);
    case PROTOBUF_C_TYPE_ENUM:
    {
      const ProtobufCEnumValue *protobuf_enum_value = protobuf_c_enum_descriptor_get_value(
        field_descriptor->descriptor,
        *(int *)protobuf_value
      );

      if (protobuf_enum_value) {
        return json_string((char *)protobuf_enum_value->name);
      } else {
        return json_string((char *)"unknown enum value");
      }
    }
    case PROTOBUF_C_TYPE_STRING:
      return json_string(*(char **)protobuf_value);
    /*case PROTOBUF_C_TYPE_BYTES:
    {
      const ProtobufCBinaryData *protobuf_binary_data = (const ProtobufCBinaryData *)protobuf_value;
      return json_stringn(protobuf_binary_data->data, protobuf_binary_data->len);
    }*/
    case PROTOBUF_C_TYPE_MESSAGE:
    {
      const ProtobufCMessage **protobuf_message = (const ProtobufCMessage **)protobuf_value;
      return protobuf2json_process_message(*protobuf_message);
    }
    /* case PROTOBUF_C_TYPE_GROUP: - NOT SUPPORTED */
    default:
      assert(0);
      return NULL;
  }
}

static json_t* protobuf2json_process_message(const ProtobufCMessage *protobuf_message) {
  json_t *json_message = json_object();
  if (!json_message) {
    return NULL;
  }

  unsigned i;
  for (i = 0; i < protobuf_message->descriptor->n_fields; i++) {
    const ProtobufCFieldDescriptor *field_descriptor = protobuf_message->descriptor->fields + i;
    const void *protobuf_value = ((const char *)protobuf_message) + field_descriptor->offset;
    const void *protobuf_value_quantifier = ((const char *)protobuf_message) + field_descriptor->quantifier_offset;

    if (field_descriptor->label == PROTOBUF_C_LABEL_REQUIRED) {
      json_t *json_value = protobuf2json_process_field(field_descriptor, protobuf_value);
      if (!json_value) {
        return NULL;
      }
      if (json_object_set_new(json_message, field_descriptor->name, json_value)) {
        return NULL;
      }
    } else if (field_descriptor->label == PROTOBUF_C_LABEL_OPTIONAL) {
      protobuf_c_boolean is_set = 0;

      if (field_descriptor->type == PROTOBUF_C_TYPE_MESSAGE || field_descriptor->type == PROTOBUF_C_TYPE_STRING) {
        if (*(const void * const *)protobuf_value) {
          is_set = 1;
        }
      } else {
        if (*(const protobuf_c_boolean *)protobuf_value_quantifier) {
          is_set = 1;
        }
      }

      if (is_set || field_descriptor->default_value) {
        json_t *json_value = protobuf2json_process_field(field_descriptor, protobuf_value);
        if (!json_value) {
          return NULL;
        }
        if (json_object_set_new(json_message, field_descriptor->name, json_value)) {
          return NULL;
        }
      }
    } else { // PROTOBUF_C_LABEL_REPEATED
      const size_t *protobuf_values_count = (const size_t *)protobuf_value_quantifier;

      if (*protobuf_values_count) {
        json_t *array = json_array();
        if (!array) {
          return NULL;
        }

        size_t value_size = protobuf2json_value_size_by_type(field_descriptor->type);
        if (!value_size) {
          return NULL;
        }

        unsigned j;
        for (j = 0; j < *protobuf_values_count; j++) {
          const char *protobuf_value_repeated = (*(char * const *)protobuf_value) + j * value_size;

          json_t* json_value = protobuf2json_process_field(field_descriptor, (const void *)protobuf_value_repeated);
          if (!json_value) {
            return NULL;
          }
          if (json_array_append_new(array, json_value)) {
            return NULL;
          }
        }

        if (json_object_set_new(json_message, field_descriptor->name, array)) {
          return NULL;
        }
      }
    }
  }

  return json_message;
}

/* === Protobuf -> JSON === Public === */

int protobuf2json_object(ProtobufCMessage *protobuf_message, json_t **json_object) {
  *json_object = protobuf2json_process_message(protobuf_message);
  if (!*json_object) {
    return PROTOBUF2JSON_ERR_CANNOT_PROCESS_MESSAGE;
  }

  return 0;
}

int protobuf2json_string(ProtobufCMessage *protobuf_message, size_t flags, char **json_string) {
  json_t *json_object = NULL;

  int ret = protobuf2json_object(protobuf_message, &json_object);
  if (ret) {
    json_decref(json_object);
    return ret;
  }

  // NOTICE: Should be freed by caller
  *json_string = json_dumps(json_object, flags);
  if (!*json_string) {
    json_decref(json_object);
    return PROTOBUF2JSON_ERR_CANNOT_DUMP_STRING;
  }

  return 0;
}

/* === JSON -> Protobuf === Private === */

int json2protobuf_process_message(
  json_t *json_object,
  const ProtobufCMessageDescriptor *protobuf_message_descriptor,
  ProtobufCMessage **protobuf_message,
  char *error_string,
  size_t error_size
);

int json2protobuf_process_field(
  const ProtobufCFieldDescriptor *field_descriptor,
  json_t *json_value,
  void *protobuf_value,
  char *error_string,
  size_t error_size
) {
  if (field_descriptor->type == PROTOBUF_C_TYPE_MESSAGE) {
    ProtobufCMessage *protobuf_message;

    int result = json2protobuf_process_message(json_value, field_descriptor->descriptor, &protobuf_message, NULL, 0);
    if (result) {
      return result;
    }

    memcpy(protobuf_value, &protobuf_message, sizeof(protobuf_message));
  } else if (field_descriptor->type == PROTOBUF_C_TYPE_ENUM) {
    if (!json_is_string(json_value)) {
      if (error_string && error_size) {
        snprintf(
          error_string, error_size,
          "JSON value is not a string required for GPB enum"
        );
      }

      return PROTOBUF2JSON_ERR_IS_NOT_STRING;
    }

    const char* enum_value_name = json_string_value(json_value);

    const ProtobufCEnumValue *enum_value;

    enum_value = protobuf_c_enum_descriptor_get_value_by_name(field_descriptor->descriptor, enum_value_name);
    if (!enum_value) {
      return PROTOBUF2JSON_ERR_TODO;
    }

    int32_t value = (int32_t)enum_value->value;

    memcpy(protobuf_value, &value, sizeof(value));
  } else if (field_descriptor->type == PROTOBUF_C_TYPE_INT32) {
    if (!json_is_integer(json_value)) {
      if (error_string && error_size) {
        snprintf(
          error_string, error_size,
          "JSON value is not an integer required for GPB int32"
        );
      }

      return PROTOBUF2JSON_ERR_IS_NOT_INTEGER;
    }

    int32_t value = (int32_t)json_integer_value(json_value);

    memcpy(protobuf_value, &value, sizeof(value));
  } else if (field_descriptor->type == PROTOBUF_C_TYPE_STRING) {
    if (!json_is_string(json_value)) {
      if (error_string && error_size) {
        snprintf(
          error_string, error_size,
          "JSON value is not a string required for GPB string"
        );
      }

      return PROTOBUF2JSON_ERR_IS_NOT_STRING;
    }

    const char* value = json_string_value(json_value);

    memcpy(protobuf_value, &value, sizeof(value));
  }

  return 0;
}

int json2protobuf_process_message(
  json_t *json_object,
  const ProtobufCMessageDescriptor *protobuf_message_descriptor,
  ProtobufCMessage **protobuf_message,
  char *error_string,
  size_t error_size
) {
  if (!json_is_object(json_object)) {
    if (error_string && error_size) {
      snprintf(
        error_string, error_size,
        "JSON is not an object required for GPB message"
      );
    }

    return PROTOBUF2JSON_ERR_IS_NOT_OBJECT;
  }

  *protobuf_message = calloc(1, protobuf_message_descriptor->sizeof_message);
  if (!*protobuf_message) {
    if (error_string && error_size) {
      snprintf(
        error_string, error_size,
        "Cannot allocate %zu bytes using calloc(3)",
        protobuf_message_descriptor->sizeof_message
      );
    }

    return PROTOBUF2JSON_ERR_CANNOT_ALLOCATE_MEMORY;
  }

  protobuf_c_message_init(protobuf_message_descriptor, *protobuf_message);

  const char *json_key;
  json_t *json_object_value;
  json_object_foreach(json_object, json_key, json_object_value) {
    const ProtobufCFieldDescriptor *field_descriptor = protobuf_c_message_descriptor_get_field_by_name(protobuf_message_descriptor, json_key);
    if (!field_descriptor) {
      if (error_string && error_size) {
        snprintf(
          error_string, error_size,
          "Unknown field '%s' for message '%s'",
          json_key, protobuf_message_descriptor->name
        );
      }

      return PROTOBUF2JSON_ERR_UNKNOWN_FIELD;
    }

    void *protobuf_value = ((char *)*protobuf_message) + field_descriptor->offset;
    void *protobuf_value_quantifier = ((char *)*protobuf_message) + field_descriptor->quantifier_offset;

    if (field_descriptor->label == PROTOBUF_C_LABEL_REQUIRED) {
      int result = json2protobuf_process_field(field_descriptor, json_object_value, protobuf_value, error_string, error_size);
      if (result) {
        return result;
      }
    } else if (field_descriptor->label == PROTOBUF_C_LABEL_OPTIONAL) {
      if (field_descriptor->type == PROTOBUF_C_TYPE_MESSAGE || field_descriptor->type == PROTOBUF_C_TYPE_STRING) {
        // Do nothing
      } else {
        *(protobuf_c_boolean *)protobuf_value_quantifier = 1;
      }

      int result = json2protobuf_process_field(field_descriptor, json_object_value, protobuf_value, error_string, error_size);
      if (result) {
        return result;
      }
    } else { // PROTOBUF_C_LABEL_REPEATED
      if (!json_is_array(json_object_value)) {
        if (error_string && error_size) {
          snprintf(
            error_string, error_size,
            "JSON is not an array required for repeatable GPB field"
          );
        }

        return PROTOBUF2JSON_ERR_IS_NOT_ARRAY;
      }

      size_t *protobuf_values_count = (size_t *)protobuf_value_quantifier;

      *protobuf_values_count = json_array_size(json_object_value);

      if (*protobuf_values_count) {
        size_t value_size = protobuf2json_value_size_by_type(field_descriptor->type);
        if (!value_size) {
          if (error_string && error_size) {
            snprintf(
              error_string, error_size,
              "Cannot calculate value size for %d using protobuf2json_value_size_by_type()",
              field_descriptor->type
            );
          }

          return PROTOBUF2JSON_ERR_UNSUPPORTED_FIELD_TYPE;
        }

        void *protobuf_value_repeated = calloc(*protobuf_values_count, value_size);
        if (!protobuf_value_repeated) {
          if (error_string && error_size) {
            snprintf(
              error_string, error_size,
              "Cannot allocate %zu bytes using calloc(3)",
              (size_t)*protobuf_values_count * value_size
            );
          }

          return PROTOBUF2JSON_ERR_CANNOT_ALLOCATE_MEMORY;
        }

        size_t json_index;
        json_t *json_array_value;
        json_array_foreach(json_object_value, json_index, json_array_value) {
          char *protobuf_value_repeated_value = (char *)protobuf_value_repeated + json_index * value_size;

          int result = json2protobuf_process_field(field_descriptor, json_array_value, (void *)protobuf_value_repeated_value, error_string, error_size);
          if (result) {
            return result;
          }
        }

        memcpy(protobuf_value, &protobuf_value_repeated, sizeof(protobuf_value_repeated));
      }
    }
  }

  return 0;
}

/* === JSON -> Protobuf === Public === */

int json2protobuf_object(
  json_t *json_object,
  const ProtobufCMessageDescriptor *protobuf_message_descriptor,
  ProtobufCMessage **protobuf_message,
  char *error_string,
  size_t error_size
) {
  int result = json2protobuf_process_message(json_object, protobuf_message_descriptor, protobuf_message, error_string, error_size);
  if (result) {
    return result;
  }

  return 0;
}

int json2protobuf_string(
  char *json_string,
  size_t flags,
  const ProtobufCMessageDescriptor *protobuf_message_descriptor,
  ProtobufCMessage **protobuf_message,
  char *error_string,
  size_t error_size
) {
  json_t *json_object = NULL;
  json_error_t error;

  json_object = json_loads(json_string, flags, &error);
  if (!json_object) {
    if (error_string && error_size) {
      snprintf(
        error_string, error_size,
        "JSON parsing error at line %d column %d (position %d): %s",
        error.line, error.column, error.position, error.text
      );
    }

    json_decref(json_object);
    return PROTOBUF2JSON_ERR_CANNOT_PARSE_STRING;
  }

  int result = json2protobuf_object(json_object, protobuf_message_descriptor, protobuf_message, error_string, error_size);
  if (result) {
    json_decref(json_object);
    return result;
  }

  return 0;
}

int json2protobuf_file(
  char *json_file,
  size_t flags,
  const ProtobufCMessageDescriptor *protobuf_message_descriptor,
  ProtobufCMessage **protobuf_message,
  char *error_string,
  size_t error_size
) {
  json_t *json_object = NULL;
  json_error_t error;

  json_object = json_load_file(json_file, flags, &error);
  if (!json_object) {
    if (error_string && error_size) {
      snprintf(
        error_string, error_size,
        "JSON parsing error at line %d column %d (position %d): %s",
        error.line, error.column, error.position, error.text
      );
    }

    json_decref(json_object);
    return PROTOBUF2JSON_ERR_CANNOT_PARSE_FILE;
  }

  int result = json2protobuf_object(json_object, protobuf_message_descriptor, protobuf_message, error_string, error_size);
  if (result) {
    json_decref(json_object);
    return result;
  }

  return 0;
}

/* === END === */
