/*
 * Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#include <assert.h>

#include "protobuf2json.h"

/* === Protobuf -> JSON === Private === */

static size_t protobuf2json_type_size(ProtobufCType type) {
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
      return sizeof(void *);
    case PROTOBUF_C_TYPE_MESSAGE:
      return sizeof(void *);
    case PROTOBUF_C_TYPE_BYTES:
      return sizeof(ProtobufCBinaryData);
    //case PROTOBUF_C_TYPE_GROUP: // NOT SUPPORTED
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
    const void *member = ((const char *)protobuf_message) + field_descriptor->offset;
    const void *quantifier_member = ((const char *)protobuf_message) + field_descriptor->quantifier_offset;
    const size_t *quantifier = (const size_t *)quantifier_member;

    if (field_descriptor->label == PROTOBUF_C_LABEL_REQUIRED) {
      json_t *json_value = protobuf2json_process_field(field_descriptor, member);
      if (!json_value) {
        return NULL;
      }
      if (json_object_set_new(json_message, field_descriptor->name, json_value)) {
        return NULL;
      }
    } else if (field_descriptor->label == PROTOBUF_C_LABEL_OPTIONAL) {
      protobuf_c_boolean is_set = 0;

      if (field_descriptor->type == PROTOBUF_C_TYPE_MESSAGE || field_descriptor->type == PROTOBUF_C_TYPE_STRING) {
        if (*(const void * const *)member) {
          is_set = 1;
        }
      } else {
        if (*(const protobuf_c_boolean *)quantifier_member) {
          is_set = 1;
        }
      }

      if (is_set || field_descriptor->default_value) {
        json_t *json_value = protobuf2json_process_field(field_descriptor, member);
        if (!json_value) {
          return NULL;
        }
        if (json_object_set_new(json_message, field_descriptor->name, json_value)) {
          return NULL;
        }
      }
    } else { // PROTOBUF_C_LABEL_REPEATED
      if (*quantifier) {
        json_t *array = json_array();
        if (!array) {
          return NULL;
        }

        size_t field_size = protobuf2json_type_size(field_descriptor->type);
        if (!field_size) {
          return NULL;
        }

        unsigned j;
        for (j = 0; j < *quantifier; j++) {
          const char *protobuf_message_repeated = (*(char * const *)member) + j * field_size;

          json_t* json_value = protobuf2json_process_field(field_descriptor, (const ProtobufCMessage *)protobuf_message_repeated);
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
    //case PROTOBUF_C_TYPE_GROUP: // NOT SUPPORTED
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

int json2protobuf_object(
  json_t *json_object,
  const ProtobufCMessageDescriptor *protobuf_message_descriptor,
  ProtobufCMessage **protobuf_message
) {
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

  if (!json_is_array(json_object)) {
    json_decref(json_object);
    return PROTOBUF2JSON_ERR_IS_NOT_ARRAY;
  }

  int ret = json2protobuf_object(json_object, protobuf_message_descriptor, protobuf_message);
  if (ret) {
    json_decref(json_object);
    return ret;
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

  if (!json_is_array(json_object)) {
    json_decref(json_object);
    return PROTOBUF2JSON_ERR_IS_NOT_ARRAY;
  }

  int ret = json2protobuf_object(json_object, protobuf_message_descriptor, protobuf_message);
  if (ret) {
    json_decref(json_object);
    return ret;
  }

  return 0;
}

/* === END === */
