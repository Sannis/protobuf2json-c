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

static json_t* protobuf2json_process_field(const ProtobufCFieldDescriptor *field_descriptor, const void *protobuf_value);

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

/* === JSON -> Protobuf === Public === */

int json2protobuf_process_field(
  ProtobufCMessage **protobuf_message,
  const ProtobufCFieldDescriptor *field_descriptor,
  json_t *json_value
) {
  void *member = ((char *)*protobuf_message) + field_descriptor->offset;
  void *quantifier_member = ((char *)*protobuf_message) + field_descriptor->quantifier_offset;

  if (field_descriptor->type == PROTOBUF_C_TYPE_INT32) {
    if (!json_is_integer(json_value)) {
      return PROTOBUF2JSON_ERR_TODO;
    }

    int32_t protobuf_value = (int32_t)json_integer_value(json_value);

    memcpy(member, &protobuf_value, sizeof(protobuf_value));
  } else if (field_descriptor->type == PROTOBUF_C_TYPE_STRING) {
    if (!json_is_string(json_value)) {
     return PROTOBUF2JSON_ERR_TODO;
    }

    const char* protobuf_value = json_string_value(json_value);

    memcpy(member, &protobuf_value, sizeof(protobuf_value));
  }

  return 0;
}

int json2protobuf_process_message(
  json_t *json_object,
  const ProtobufCMessageDescriptor *protobuf_message_descriptor,
  ProtobufCMessage **protobuf_message
) {
  *protobuf_message = calloc(1, protobuf_message_descriptor->sizeof_message);
  if (!protobuf_message) {
    return PROTOBUF2JSON_ERR_CANNOT_ALLOCATE_MEMORY;
  }

  protobuf_c_message_init(protobuf_message_descriptor, *protobuf_message);

  const char *json_key;
  json_t *json_value;
  json_object_foreach(json_object, json_key, json_value) {
    const ProtobufCFieldDescriptor *field_descriptor = protobuf_c_message_descriptor_get_field_by_name(protobuf_message_descriptor, json_key);
    if (!field_descriptor) {
      // Unknown field
      return PROTOBUF2JSON_ERR_UNKNOWN_FIELD;
    }

    if (field_descriptor->type == PROTOBUF_C_TYPE_MESSAGE) {
      return PROTOBUF2JSON_ERR_TODO;
    } else if (field_descriptor->type == PROTOBUF_C_TYPE_ENUM) {
      const ProtobufCEnumValue *enum_value;

      enum_value = protobuf_c_enum_descriptor_get_value_by_name((const ProtobufCEnumDescriptor *)field_descriptor, json_key);
      if (!enum_value) {
        return PROTOBUF2JSON_ERR_TODO;
      }

      return PROTOBUF2JSON_ERR_TODO;
    } else {
      int result = json2protobuf_process_field(protobuf_message, field_descriptor, json_value);
      if (result) {
        return result;
      }
    }
  }

  return 0;
}

int json2protobuf_object(
  json_t *json_object,
  const ProtobufCMessageDescriptor *protobuf_message_descriptor,
  ProtobufCMessage **protobuf_message
) {
  if (!json_is_object(json_object)) {
    return PROTOBUF2JSON_ERR_IS_NOT_OBJECT;
  }

  int result = json2protobuf_process_message(json_object, protobuf_message_descriptor, protobuf_message);
  if (result) {
    return result;
  }

  return 0;
}

int json2protobuf_string(
  char *json_string,
  size_t flags,
  const ProtobufCMessageDescriptor *protobuf_message_descriptor,
  ProtobufCMessage **protobuf_message
  /*, json_error_t *json_error*/
) {
  json_t *json_object = NULL;

  json_object = json_loads(json_string, flags, NULL);
  if (!json_object) {
    json_decref(json_object);
    return PROTOBUF2JSON_ERR_CANNOT_PARSE_STRING;
  }

  int result = json2protobuf_object(json_object, protobuf_message_descriptor, protobuf_message);
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
  ProtobufCMessage **protobuf_message
  /*, json_error_t *json_error*/
) {
  json_t *json_object = NULL;

  json_object = json_load_file(json_file, flags, NULL);
  if (!json_object) {
    json_decref(json_object);
    return PROTOBUF2JSON_ERR_CANNOT_PARSE_FILE;
  }

  int result = json2protobuf_object(json_object, protobuf_message_descriptor, protobuf_message);
  if (result) {
    json_decref(json_object);
    return result;
  }

  return 0;
}

/* === END === */
