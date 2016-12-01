/*
 * Copyright (c) 2014-2016 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

/* Interface definitions */
#include "protobuf2json.h"

/* Simple bitmap implementation */
#include "bitmap.h"

/* Simple base64 implementation */
#include "base64.h"

/* === Defines === obviously private === */

#define SET_ERROR_STRING_AND_RETURN(error, error_string_format, ...)                           \
do {                                                                                           \
  if (error_string && error_size) {                                                            \
    snprintf(                                                                                  \
      error_string, error_size,                                                                \
      error_string_format,                                                                     \
      ##__VA_ARGS__                                                                            \
    );                                                                                         \
  }                                                                                            \
  return error;                                                                                \
} while (0)

/* === Protobuf -> JSON === Private === */

static size_t protobuf2json_value_size_by_type(ProtobufCType type) {
  switch (type) {
    case PROTOBUF_C_TYPE_INT32:
    case PROTOBUF_C_TYPE_SINT32:
    case PROTOBUF_C_TYPE_SFIXED32:
    case PROTOBUF_C_TYPE_UINT32:
    case PROTOBUF_C_TYPE_FIXED32:
      return 4;
    case PROTOBUF_C_TYPE_INT64:
    case PROTOBUF_C_TYPE_SINT64:
    case PROTOBUF_C_TYPE_SFIXED64:
    case PROTOBUF_C_TYPE_UINT64:
    case PROTOBUF_C_TYPE_FIXED64:
      return 8;
    case PROTOBUF_C_TYPE_FLOAT:
      return 4;
    case PROTOBUF_C_TYPE_DOUBLE:
      return 8;
    case PROTOBUF_C_TYPE_BOOL:
      return sizeof(protobuf_c_boolean);
    case PROTOBUF_C_TYPE_ENUM:
      return 4;
    case PROTOBUF_C_TYPE_STRING:
      return sizeof(char *);
    case PROTOBUF_C_TYPE_BYTES:
      return sizeof(ProtobufCBinaryData);
    case PROTOBUF_C_TYPE_MESSAGE:
      return sizeof(ProtobufCMessage *);
    default:
      assert(0);
      return 0;
  }
}

static int protobuf2json_process_message(
  const ProtobufCMessage *protobuf_message,
  json_t **json_message,
  char *error_string,
  size_t error_size
);

static int protobuf2json_process_field(
  const ProtobufCFieldDescriptor *field_descriptor,
  const void *protobuf_value,
  json_t **json_value,
  char *error_string,
  size_t error_size
) {
  switch (field_descriptor->type) {
    case PROTOBUF_C_TYPE_INT32:
    case PROTOBUF_C_TYPE_SINT32:
    case PROTOBUF_C_TYPE_SFIXED32:
      *json_value = json_integer(*(int32_t *)protobuf_value);
      break;
    case PROTOBUF_C_TYPE_UINT32:
    case PROTOBUF_C_TYPE_FIXED32:
      *json_value = json_integer(*(uint32_t *)protobuf_value);
      break;
    case PROTOBUF_C_TYPE_INT64:
    case PROTOBUF_C_TYPE_SINT64:
    case PROTOBUF_C_TYPE_SFIXED64:
      *json_value = json_integer(*(int64_t *)protobuf_value);
      break;
    case PROTOBUF_C_TYPE_UINT64:
    case PROTOBUF_C_TYPE_FIXED64:
      *json_value = json_integer(*(uint64_t *)protobuf_value);
      break;
    case PROTOBUF_C_TYPE_FLOAT:
      *json_value = json_real(*(float *)protobuf_value);
      break;
    case PROTOBUF_C_TYPE_DOUBLE:
      *json_value = json_real(*(double *)protobuf_value);
      break;
    case PROTOBUF_C_TYPE_BOOL:
      *json_value = json_boolean(*(protobuf_c_boolean *)protobuf_value);
      break;
    case PROTOBUF_C_TYPE_ENUM: {
      const ProtobufCEnumValue *protobuf_enum_value = protobuf_c_enum_descriptor_get_value(
        field_descriptor->descriptor,
        *(int *)protobuf_value
      );

      if (!protobuf_enum_value) {
        SET_ERROR_STRING_AND_RETURN(
          PROTOBUF2JSON_ERR_UNKNOWN_ENUM_VALUE,
          "Unknown value %d for enum '%s'",
          *(int *)protobuf_value, ((ProtobufCEnumDescriptor *)field_descriptor->descriptor)->name
        );
      }

      *json_value = json_string((char *)protobuf_enum_value->name);

      break;
    }
    case PROTOBUF_C_TYPE_STRING:
      *json_value = json_string(*(char **)protobuf_value);
      break;
    case PROTOBUF_C_TYPE_BYTES: {
      const ProtobufCBinaryData *protobuf_binary = (const ProtobufCBinaryData *)protobuf_value;

      int base64_encoded_length = base64_encoded_len(protobuf_binary->len);

      char* base64_encoded_data = calloc(base64_encoded_length, sizeof(char));
      if (!base64_encoded_data) {
        SET_ERROR_STRING_AND_RETURN(
          PROTOBUF2JSON_ERR_CANNOT_ALLOCATE_MEMORY,
          "Cannot allocate %zu bytes using calloc(3)",
          base64_encoded_length * sizeof(char)
        );
      }

      base64_encoded_length = base64_encode(base64_encoded_data, (const char *)protobuf_binary->data, protobuf_binary->len);

      *json_value = json_stringn((const char *)base64_encoded_data, base64_encoded_length);

      free(base64_encoded_data);

      break;
    }
    case PROTOBUF_C_TYPE_MESSAGE: {
      const ProtobufCMessage **protobuf_message = (const ProtobufCMessage **)protobuf_value;

      int result = protobuf2json_process_message(*protobuf_message, json_value, error_string, error_size);
      if (result) {
        return result;
      }

      break;
    }
    default:
      assert(0);
  }

  if (!*json_value) {
    SET_ERROR_STRING_AND_RETURN(
      PROTOBUF2JSON_ERR_CANNOT_ALLOCATE_MEMORY,
      "Cannot allocate JSON structure in protobuf2json_process_field()"
    );
  }

  return 0;
}

static int protobuf2json_process_message(
  const ProtobufCMessage *protobuf_message,
  json_t **json_message,
  char *error_string,
  size_t error_size
) {
  *json_message = json_object();
  if (!*json_message) {
    SET_ERROR_STRING_AND_RETURN(
      PROTOBUF2JSON_ERR_CANNOT_ALLOCATE_MEMORY,
      "Cannot allocate JSON structure using json_object()"
    );
  }

  json_t *json_value = NULL;

  unsigned i;
  for (i = 0; i < protobuf_message->descriptor->n_fields; i++) {
    const ProtobufCFieldDescriptor *field_descriptor = protobuf_message->descriptor->fields + i;
    const void *protobuf_value = ((const char *)protobuf_message) + field_descriptor->offset;
    const void *protobuf_value_quantifier = ((const char *)protobuf_message) + field_descriptor->quantifier_offset;

    if (field_descriptor->label == PROTOBUF_C_LABEL_REQUIRED) {
      json_value = NULL;

      int result = protobuf2json_process_field(field_descriptor, protobuf_value, &json_value, error_string, error_size);
      if (result) {
        return result;
      }

      if (json_object_set_new(*json_message, field_descriptor->name, json_value)) {
        SET_ERROR_STRING_AND_RETURN(
          PROTOBUF2JSON_ERR_JANSSON_INTERNAL,
          "Error in json_object_set_new()"
        );
      }
    } else if (field_descriptor->label == PROTOBUF_C_LABEL_OPTIONAL) {
      if (field_descriptor->flags & PROTOBUF_C_FIELD_FLAG_ONEOF) {
        if (*(uint32_t *)protobuf_value_quantifier == field_descriptor->id) {
          if (field_descriptor->type == PROTOBUF_C_TYPE_MESSAGE || field_descriptor->type == PROTOBUF_C_TYPE_STRING) {
            if (protobuf_value == NULL || protobuf_value == field_descriptor->default_value) {
              continue;
            }
          }
        } else {
          continue;
        }
      }

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
        json_value = NULL;

        int result = protobuf2json_process_field(field_descriptor, protobuf_value, &json_value, error_string, error_size);
        if (result) {
          return result;
        }

        if (json_object_set_new(*json_message, field_descriptor->name, json_value)) {
          SET_ERROR_STRING_AND_RETURN(
            PROTOBUF2JSON_ERR_JANSSON_INTERNAL,
            "Error in json_object_set_new()"
          );
        }
      }
    } else { // PROTOBUF_C_LABEL_REPEATED
      const size_t *protobuf_values_count = (const size_t *)protobuf_value_quantifier;

      if (*protobuf_values_count) {
        json_t *array = json_array();
        if (!array) {
          SET_ERROR_STRING_AND_RETURN(
            PROTOBUF2JSON_ERR_CANNOT_ALLOCATE_MEMORY,
            "Cannot allocate JSON structure using json_array()"
          );
        }

        size_t value_size = protobuf2json_value_size_by_type(field_descriptor->type);
        if (!value_size) {
          SET_ERROR_STRING_AND_RETURN(
            PROTOBUF2JSON_ERR_UNSUPPORTED_FIELD_TYPE,
            "Cannot calculate value size for %d using protobuf2json_value_size_by_type()",
            field_descriptor->type
          );
        }

        unsigned j;
        for (j = 0; j < *protobuf_values_count; j++) {
          const char *protobuf_value_repeated = (*(char * const *)protobuf_value) + j * value_size;

          json_value = NULL;

          int result = protobuf2json_process_field(field_descriptor, (const void *)protobuf_value_repeated, &json_value, error_string, error_size);
          if (result) {
            return result;
          }

          if (json_array_append_new(array, json_value)) {
            SET_ERROR_STRING_AND_RETURN(
              PROTOBUF2JSON_ERR_JANSSON_INTERNAL,
              "Error in json_array_append_new()"
            );
          }
        }

        if (json_object_set_new(*json_message, field_descriptor->name, array)) {
          SET_ERROR_STRING_AND_RETURN(
            PROTOBUF2JSON_ERR_JANSSON_INTERNAL,
            "Error in json_object_set_new()"
          );
        }
      }
    }
  }

  return 0;
}

/* === Protobuf -> JSON === Public === */

int protobuf2json_object(
  ProtobufCMessage *protobuf_message,
  json_t **json_object,
  char *error_string,
  size_t error_size
) {
  int ret = protobuf2json_process_message(protobuf_message, json_object, error_string, error_size);
  if (ret) {
    json_decref(*json_object);
    return ret;
  }

  return 0;
}

int protobuf2json_string(
  ProtobufCMessage *protobuf_message,
  size_t json_flags,
  char **json_string,
  char *error_string,
  size_t error_size
) {
  json_t *json_object = NULL;

  int ret = protobuf2json_object(protobuf_message, &json_object, error_string, error_size);
  if (ret) {
    return ret;
  }

  // NOTICE: Should be freed by caller
  *json_string = json_dumps(json_object, json_flags);
  if (!*json_string) {
    json_decref(json_object);

    SET_ERROR_STRING_AND_RETURN(
      PROTOBUF2JSON_ERR_CANNOT_DUMP_STRING,
      "Cannot dump JSON object to string using json_dumps()"
    );
  }

  json_decref(json_object);
  return 0;
}

int protobuf2json_file(
  ProtobufCMessage *protobuf_message,
  size_t json_flags,
  char *json_file,
  char *fopen_mode,
  char *error_string,
  size_t error_size
) {
  char *json_string = NULL;
  FILE *fd = NULL;

  if (!json_file) {
    SET_ERROR_STRING_AND_RETURN(
      PROTOBUF2JSON_ERR_CANNOT_DUMP_FILE,
      "Cannot open NULL to dump JSON"
    );
  }

  if (!fopen_mode) {
    SET_ERROR_STRING_AND_RETURN(
      PROTOBUF2JSON_ERR_CANNOT_DUMP_FILE,
      "Cannot open file with NULL fopen(3) mode to dump JSON"
    );
  }

  int ret = protobuf2json_string(protobuf_message, json_flags, &json_string, error_string, error_size);
  if (ret) {
    return ret;
  }

  fd = fopen(json_file, fopen_mode);
  if (!fd) {
    free(json_string);

    SET_ERROR_STRING_AND_RETURN(
      PROTOBUF2JSON_ERR_CANNOT_DUMP_FILE,
      "Cannot open file '%s' with mode '%s' to dump JSON, errno=%d",
      json_file, fopen_mode, errno
    );
  }

  if (fwrite(json_string, strlen(json_string), 1, fd) != 1) {
    fclose(fd);
    free(json_string);

    SET_ERROR_STRING_AND_RETURN(
      PROTOBUF2JSON_ERR_CANNOT_DUMP_FILE,
      "Cannot write JSON to file '%s' with mode '%s', errno=%d",
      json_file, fopen_mode, errno
    );
  }

  fclose(fd);
  free(json_string);
  return 0;
}

/* === JSON -> Protobuf === Private === */

static int json2protobuf_process_message(
  json_t *json_object,
  const ProtobufCMessageDescriptor *protobuf_message_descriptor,
  ProtobufCMessage **protobuf_message,
  char *error_string,
  size_t error_size
);

static const char* json2protobuf_integer_name_by_c_type(ProtobufCType type) {
  switch (type) {
    case PROTOBUF_C_TYPE_INT32:
      return "int32";
    case PROTOBUF_C_TYPE_SINT32:
      return "sint32";
    case PROTOBUF_C_TYPE_SFIXED32:
      return "sfixed32";
    case PROTOBUF_C_TYPE_UINT32:
      return "uint32";
    case PROTOBUF_C_TYPE_FIXED32:
      return "fixed32";
    case PROTOBUF_C_TYPE_INT64:
      return "int64";
    case PROTOBUF_C_TYPE_SINT64:
      return "sint64";
    case PROTOBUF_C_TYPE_SFIXED64:
      return "sfixed64";
    case PROTOBUF_C_TYPE_UINT64:
      return "uint64";
    case PROTOBUF_C_TYPE_FIXED64:
      return "fixed64";
    default:
      assert(0);
      return "unknown";
  }
}

static int json2protobuf_process_field(
  const ProtobufCFieldDescriptor *field_descriptor,
  json_t *json_value,
  void *protobuf_value,
  char *error_string,
  size_t error_size
) {
  if (field_descriptor->type == PROTOBUF_C_TYPE_INT32
   || field_descriptor->type == PROTOBUF_C_TYPE_SINT32
   || field_descriptor->type == PROTOBUF_C_TYPE_SFIXED32
  ) {
    if (!json_is_integer(json_value)) {
      SET_ERROR_STRING_AND_RETURN(
        PROTOBUF2JSON_ERR_IS_NOT_INTEGER,
        "JSON value is not an integer required for GPB %s",
        json2protobuf_integer_name_by_c_type(field_descriptor->type)
      );
    }

    int32_t value_int32_t = (int32_t)json_integer_value(json_value);

    memcpy(protobuf_value, &value_int32_t, sizeof(value_int32_t));
  } else if (field_descriptor->type == PROTOBUF_C_TYPE_UINT32
          || field_descriptor->type == PROTOBUF_C_TYPE_FIXED32
  ) {
    if (!json_is_integer(json_value)) {
      SET_ERROR_STRING_AND_RETURN(
        PROTOBUF2JSON_ERR_IS_NOT_INTEGER,
        "JSON value is not an integer required for GPB %s",
        json2protobuf_integer_name_by_c_type(field_descriptor->type)
      );
    }

    uint32_t value_uint32_t = (uint32_t)json_integer_value(json_value);

    memcpy(protobuf_value, &value_uint32_t, sizeof(value_uint32_t));
  } else if (field_descriptor->type == PROTOBUF_C_TYPE_INT64
          || field_descriptor->type == PROTOBUF_C_TYPE_SINT64
          || field_descriptor->type == PROTOBUF_C_TYPE_SFIXED64
  ) {
    if (!json_is_integer(json_value)) {
      SET_ERROR_STRING_AND_RETURN(
        PROTOBUF2JSON_ERR_IS_NOT_INTEGER,
        "JSON value is not an integer required for GPB %s",
        json2protobuf_integer_name_by_c_type(field_descriptor->type)
      );
    }

    int64_t value_int64_t = (int64_t)json_integer_value(json_value);

    memcpy(protobuf_value, &value_int64_t, sizeof(value_int64_t));
  } else if (field_descriptor->type == PROTOBUF_C_TYPE_UINT64
          || field_descriptor->type == PROTOBUF_C_TYPE_FIXED64
  ) {
    if (!json_is_integer(json_value)) {
      SET_ERROR_STRING_AND_RETURN(
        PROTOBUF2JSON_ERR_IS_NOT_INTEGER,
        "JSON value is not an integer required for GPB %s",
        json2protobuf_integer_name_by_c_type(field_descriptor->type)
      );
    }

    uint64_t value_uint64_t = (uint64_t)json_integer_value(json_value);

    memcpy(protobuf_value, &value_uint64_t, sizeof(value_uint64_t));
  } else if (field_descriptor->type == PROTOBUF_C_TYPE_FLOAT) {
    float value_float;

    if (json_is_integer(json_value)) {
      value_float = (float)json_integer_value(json_value);
    } else if (json_is_real(json_value)) {
      value_float = (float)json_real_value(json_value);
    } else {
      SET_ERROR_STRING_AND_RETURN(
        PROTOBUF2JSON_ERR_IS_NOT_INTEGER_OR_REAL,
        "JSON value is not a integer/real required for GPB float"
      );
    }

    memcpy(protobuf_value, &value_float, sizeof(value_float));
  } else if (field_descriptor->type == PROTOBUF_C_TYPE_DOUBLE) {
    double value_double;

    if (json_is_integer(json_value)) {
      value_double = (double)json_integer_value(json_value);
    } else if (json_is_real(json_value)) {
      value_double = (double)json_real_value(json_value);
    } else {
      SET_ERROR_STRING_AND_RETURN(
        PROTOBUF2JSON_ERR_IS_NOT_INTEGER_OR_REAL,
        "JSON value is not a integer/real required for GPB double"
      );
    }

    memcpy(protobuf_value, &value_double, sizeof(value_double));
  } else if (field_descriptor->type == PROTOBUF_C_TYPE_BOOL) {
    if (!json_is_boolean(json_value)) {
      SET_ERROR_STRING_AND_RETURN(
        PROTOBUF2JSON_ERR_IS_NOT_BOOLEAN,
        "JSON value is not a boolean required for GPB bool"
      );
    }

    protobuf_c_boolean value_boolean = (protobuf_c_boolean)json_boolean_value(json_value);

    memcpy(protobuf_value, &value_boolean, sizeof(value_boolean));
  } else if (field_descriptor->type == PROTOBUF_C_TYPE_ENUM) {
    if (!json_is_string(json_value)) {
      SET_ERROR_STRING_AND_RETURN(
        PROTOBUF2JSON_ERR_IS_NOT_STRING,
        "JSON value is not a string required for GPB enum"
      );
    }

    const char* enum_value_name = json_string_value(json_value);

    const ProtobufCEnumValue *enum_value;

    enum_value = protobuf_c_enum_descriptor_get_value_by_name(field_descriptor->descriptor, enum_value_name);
    if (!enum_value) {
      SET_ERROR_STRING_AND_RETURN(
        PROTOBUF2JSON_ERR_UNKNOWN_ENUM_VALUE,
        "Unknown value '%s' for enum '%s'",
        enum_value_name, ((ProtobufCEnumDescriptor *)field_descriptor->descriptor)->name
      );
    }

    int32_t value_enum = (int32_t)enum_value->value;

    memcpy(protobuf_value, &value_enum, sizeof(value_enum));
  } else if (field_descriptor->type == PROTOBUF_C_TYPE_STRING) {
    if (!json_is_string(json_value)) {
      SET_ERROR_STRING_AND_RETURN(
        PROTOBUF2JSON_ERR_IS_NOT_STRING,
        "JSON value is not a string required for GPB string"
      );
    }

    const char* value_string = json_string_value(json_value);
    size_t value_string_length = strlen(value_string);

    char* value_string_copy = calloc(value_string_length + 1, sizeof(char));
    if (!value_string_copy) {
      SET_ERROR_STRING_AND_RETURN(
        PROTOBUF2JSON_ERR_CANNOT_ALLOCATE_MEMORY,
        "Cannot allocate %zu bytes using calloc(3)",
        (value_string_length + 1) * sizeof(char)
      );
    }

    memcpy(value_string_copy, value_string, value_string_length + 1);

    *(char **)(protobuf_value) = value_string_copy;
  } else if (field_descriptor->type == PROTOBUF_C_TYPE_BYTES) {
    if (!json_is_string(json_value)) {
      SET_ERROR_STRING_AND_RETURN(
        PROTOBUF2JSON_ERR_IS_NOT_STRING,
        "JSON value is not a string required for GPB bytes"
      );
    }

    const char* value_string = json_string_value(json_value);
    size_t value_string_length = json_string_length(json_value);

    int base64_decoded_length = base64_decoded_len(value_string_length);

    char* base64_decoded_data = calloc(base64_decoded_length, sizeof(char));
    if (!base64_decoded_data) {
      SET_ERROR_STRING_AND_RETURN(
        PROTOBUF2JSON_ERR_CANNOT_ALLOCATE_MEMORY,
        "Cannot allocate %zu bytes using calloc(3)",
        base64_decoded_length * sizeof(char)
      );
    }

    /* @todo: check for zero length / error */
    base64_decoded_length = base64_decode(base64_decoded_data, value_string, value_string_length);

    char* value_string_copy = calloc(base64_decoded_length, sizeof(char));
    if (!value_string_copy) {
      SET_ERROR_STRING_AND_RETURN(
        PROTOBUF2JSON_ERR_CANNOT_ALLOCATE_MEMORY,
        "Cannot allocate %zu bytes using calloc(3)",
        base64_decoded_length * sizeof(char)
      );
    }

    memcpy(value_string_copy, base64_decoded_data, base64_decoded_length);

    free(base64_decoded_data);

    ProtobufCBinaryData value_binary;

    value_binary.data = (uint8_t *)value_string_copy;
    value_binary.len = base64_decoded_length;

    memcpy(protobuf_value, &value_binary, sizeof(value_binary));
  } else if (field_descriptor->type == PROTOBUF_C_TYPE_MESSAGE) {
    ProtobufCMessage *protobuf_message;

    int result = json2protobuf_process_message(json_value, field_descriptor->descriptor, &protobuf_message, error_string, error_size);
    if (result) {
      return result;
    }

    memcpy(protobuf_value, &protobuf_message, sizeof(protobuf_message));
   } else {
    assert(0);
  }

  return 0;
}

#define SAFE_FREE_BITMAP_AND_MESSAGE                           \
do {                                                           \
  if (presented_fields) {                                      \
    bitmap_free(presented_fields);                             \
  }                                                            \
  if (protobuf_message) {                                      \
    protobuf_c_message_free_unpacked(*protobuf_message, NULL); \
    *protobuf_message = NULL;                                  \
  }                                                            \
} while (0)

static int json2protobuf_process_message(
  json_t *json_object,
  const ProtobufCMessageDescriptor *protobuf_message_descriptor,
  ProtobufCMessage **protobuf_message,
  char *error_string,
  size_t error_size
) {
  bitmap_t presented_fields = NULL;

  int result = 0;

  if (!json_is_object(json_object)) {
    SET_ERROR_STRING_AND_RETURN(
      PROTOBUF2JSON_ERR_IS_NOT_OBJECT,
      "JSON is not an object required for GPB message"
    );
  }

  *protobuf_message = calloc(1, protobuf_message_descriptor->sizeof_message);
  if (!*protobuf_message) {
    SET_ERROR_STRING_AND_RETURN(
      PROTOBUF2JSON_ERR_CANNOT_ALLOCATE_MEMORY,
      "Cannot allocate %zu bytes using calloc(3)",
      protobuf_message_descriptor->sizeof_message
    );
  }

  protobuf_c_message_init(protobuf_message_descriptor, *protobuf_message);

  presented_fields = bitmap_alloc(protobuf_message_descriptor->n_fields);
  if (!presented_fields) {
    SAFE_FREE_BITMAP_AND_MESSAGE;

    SET_ERROR_STRING_AND_RETURN(
      PROTOBUF2JSON_ERR_CANNOT_ALLOCATE_MEMORY,
      "Cannot allocate bitmap structure using bitmap_alloc()"
    );
  }

  const char *json_key;
  json_t *json_object_value;
  json_object_foreach(json_object, json_key, json_object_value) {
    const ProtobufCFieldDescriptor *field_descriptor = protobuf_c_message_descriptor_get_field_by_name(protobuf_message_descriptor, json_key);
    if (!field_descriptor) {
      SAFE_FREE_BITMAP_AND_MESSAGE;

      SET_ERROR_STRING_AND_RETURN(
        PROTOBUF2JSON_ERR_UNKNOWN_FIELD,
        "Unknown field '%s' for message '%s'",
        json_key, protobuf_message_descriptor->name
      );
    }

    unsigned int field_number = field_descriptor - protobuf_message_descriptor->fields;

    // This cannot happen because Jansson handle this on his side
    /*if (bitmap_get(presented_fields, field_number)) {
      SAFE_FREE_BITMAP_AND_MESSAGE;

      SET_ERROR_STRING_AND_RETURN(
        PROTOBUF2JSON_ERR_DUPLICATE_FIELD,
        "Duplicate field '%s' for message '%s'",
        json_key, protobuf_message_descriptor->name
      );
    }*/
    bitmap_set(presented_fields, field_number);

    void *protobuf_value = ((char *)*protobuf_message) + field_descriptor->offset;
    void *protobuf_value_quantifier = ((char *)*protobuf_message) + field_descriptor->quantifier_offset;

    if (field_descriptor->flags & PROTOBUF_C_FIELD_FLAG_ONEOF) {
      *(uint32_t*)protobuf_value_quantifier = field_descriptor->id;
    }

    if (field_descriptor->label == PROTOBUF_C_LABEL_REQUIRED) {
      result = json2protobuf_process_field(field_descriptor, json_object_value, protobuf_value, error_string, error_size);
      if (result) {
        SAFE_FREE_BITMAP_AND_MESSAGE;

        return result;
      }
    } else if (field_descriptor->label == PROTOBUF_C_LABEL_OPTIONAL) {
      if (field_descriptor->type == PROTOBUF_C_TYPE_MESSAGE || field_descriptor->type == PROTOBUF_C_TYPE_STRING
        || (field_descriptor->flags & PROTOBUF_C_FIELD_FLAG_ONEOF)) {
        // Do nothing
      } else {
        *(protobuf_c_boolean *)protobuf_value_quantifier = 1;
      }

      result = json2protobuf_process_field(field_descriptor, json_object_value, protobuf_value, error_string, error_size);
      if (result) {
        SAFE_FREE_BITMAP_AND_MESSAGE;

        return result;
      }
    } else { // PROTOBUF_C_LABEL_REPEATED
      if (!json_is_array(json_object_value)) {
        SAFE_FREE_BITMAP_AND_MESSAGE;

        SET_ERROR_STRING_AND_RETURN(
          PROTOBUF2JSON_ERR_IS_NOT_ARRAY,
          "JSON is not an array required for repeatable GPB field"
        );
      }

      size_t *protobuf_values_count = (size_t *)protobuf_value_quantifier;

      *protobuf_values_count = json_array_size(json_object_value);

      if (*protobuf_values_count) {
        size_t value_size = protobuf2json_value_size_by_type(field_descriptor->type);
        if (!value_size) {
          SAFE_FREE_BITMAP_AND_MESSAGE;

          SET_ERROR_STRING_AND_RETURN(
            PROTOBUF2JSON_ERR_UNSUPPORTED_FIELD_TYPE,
            "Cannot calculate value size for %d using protobuf2json_value_size_by_type()",
            field_descriptor->type
          );
        }

        void *protobuf_value_repeated = calloc(*protobuf_values_count, value_size);
        if (!protobuf_value_repeated) {
          SAFE_FREE_BITMAP_AND_MESSAGE;

          SET_ERROR_STRING_AND_RETURN(
            PROTOBUF2JSON_ERR_CANNOT_ALLOCATE_MEMORY,
            "Cannot allocate %zu bytes using calloc(3)",
            (size_t)*protobuf_values_count * value_size
          );
        }

        size_t json_index;
        json_t *json_array_value;
        json_array_foreach(json_object_value, json_index, json_array_value) {
          char *protobuf_value_repeated_value = (char *)protobuf_value_repeated + json_index * value_size;

          result = json2protobuf_process_field(field_descriptor, json_array_value, (void *)protobuf_value_repeated_value, error_string, error_size);
          if (result) {
            /* Free already processed repeated field items */
            {
              if (field_descriptor->type == PROTOBUF_C_TYPE_STRING) {
                size_t t;
                for (t = 0; t <= json_index; t++) {
                  free(((char **)protobuf_value_repeated)[t]);
                }
              } else if (field_descriptor->type == PROTOBUF_C_TYPE_BYTES) {
                size_t t;
                for (t = 0; t <= json_index; t++) {
                  free(((ProtobufCBinaryData *)protobuf_value_repeated)[t].data);
                }
              } else if (field_descriptor->type == PROTOBUF_C_TYPE_MESSAGE) {
                size_t t;
                for (t = 0; t <= json_index; t++) {
                  if (((ProtobufCMessage **)protobuf_value_repeated)[t]) {
                    protobuf_c_message_free_unpacked(
                      ((ProtobufCMessage **)protobuf_value_repeated)[t],
                      NULL
                    );
                  }
                }
              }

              free(protobuf_value_repeated);
              *protobuf_values_count = 0;
            }

            SAFE_FREE_BITMAP_AND_MESSAGE;

            return result;
          }
        }

        memcpy(protobuf_value, &protobuf_value_repeated, sizeof(protobuf_value_repeated));
      }
    }
  }

  unsigned int i = 0;
  for (i = 0; i < protobuf_message_descriptor->n_fields; i++) {
    const ProtobufCFieldDescriptor *field_descriptor = protobuf_message_descriptor->fields + i;

    if ((field_descriptor->label == PROTOBUF_C_LABEL_REQUIRED) && !field_descriptor->default_value && !bitmap_get(presented_fields, i)) {
      SAFE_FREE_BITMAP_AND_MESSAGE;

      SET_ERROR_STRING_AND_RETURN(
        PROTOBUF2JSON_ERR_REQUIRED_IS_MISSING,
        "Required field '%s' is missing in message '%s'",
        field_descriptor->name, protobuf_message_descriptor->name
      );
    }
  }

  bitmap_free(presented_fields);

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
  size_t json_flags,
  const ProtobufCMessageDescriptor *protobuf_message_descriptor,
  ProtobufCMessage **protobuf_message,
  char *error_string,
  size_t error_size
) {
  json_t *json_object = NULL;
  json_error_t error;

  json_object = json_loads(json_string, json_flags, &error);
  if (!json_object) {
    json_decref(json_object);

    SET_ERROR_STRING_AND_RETURN(
      PROTOBUF2JSON_ERR_CANNOT_PARSE_STRING,
      "JSON parsing error at line %d column %d (position %d): %s",
      error.line, error.column, error.position, error.text
    );
  }

  int result = json2protobuf_object(json_object, protobuf_message_descriptor, protobuf_message, error_string, error_size);
  if (result) {
    json_decref(json_object);
    return result;
  }

  json_decref(json_object);
  return 0;
}

int json2protobuf_file(
  char *json_file,
  size_t json_flags,
  const ProtobufCMessageDescriptor *protobuf_message_descriptor,
  ProtobufCMessage **protobuf_message,
  char *error_string,
  size_t error_size
) {
  json_t *json_object = NULL;
  json_error_t error;

  json_object = json_load_file(json_file, json_flags, &error);
  if (!json_object) {
    json_decref(json_object);

    SET_ERROR_STRING_AND_RETURN(
      PROTOBUF2JSON_ERR_CANNOT_PARSE_FILE,
      "JSON parsing error at line %d column %d (position %d): %s",
      error.line, error.column, error.position, error.text
    );
  }

  int result = json2protobuf_object(json_object, protobuf_message_descriptor, protobuf_message, error_string, error_size);
  if (result) {
    json_decref(json_object);
    return result;
  }

  return 0;
}

/* === END === */
