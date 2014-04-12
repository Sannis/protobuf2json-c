/*
 * Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#include "task.h"
#include "person.pb-c.h"
#include "test.pb-c.h"
#include "protobuf2json.h"

int person__error_unknown_enum_value(void) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42,\n"
    "  \"phone\": [\n"
    "    {\n"
    "      \"number\": \"+123456789\",\n"
    "      \"type\": \"UNKNOWN\"\n"
    "    }\n"
    "  ]\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  //asm volatile ("int $3");

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__person__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT(result == PROTOBUF2JSON_ERR_UNKNOWN_ENUM_VALUE);

  const char *expected_error_string = \
    "Unknown value 'UNKNOWN' for enum 'Foo.Person.PhoneType'"
  ;

  printf("Error: %s\n", error_string);

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  return 0;
}

int list(void) {
  int result;

  const char *initial_json_string = \
    "{\n"
    "  \"numbers\": [\n"
    "    12,\n"
    "    34,\n"
    "    56\n"
    "  ]\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__list__descriptor, &protobuf_message, NULL, 0);
  ASSERT(result == 0);

  Foo__List *list = (Foo__List *)protobuf_message;

  //asm volatile ("int $3");

  printf("list->n_numbers: %zu\n", list->n_numbers);
  printf("list->numbers[0]: %d\n", list->numbers[0]);
  printf("list->numbers[1]: %d\n", list->numbers[1]);
  printf("list->numbers[2]: %d\n", list->numbers[2]);

  ASSERT(list->n_numbers == 3);
  ASSERT(list->numbers[0] == 12);
  ASSERT(list->numbers[1] == 34);
  ASSERT(list->numbers[2] == 56);

  char *json_string;
  result = protobuf2json_string(protobuf_message, JSON_INDENT(2), &json_string);
  ASSERT(result == 0);
  ASSERT(json_string);

  const char *expected_json_string = \
    "{\n"
    "  \"numbers\": [\n"
    "    12,\n"
    "    34,\n"
    "    56\n"
    "  ]\n"
    "}"
  ;

  ASSERT_STRCMP(
    json_string,
    expected_json_string
  );

  return 0;
}

int person(void) {
  int result;

  const char *initial_json_string = \
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42,\n"
    "  \"phone\": [\n"
    "    {\n"
    "      \"number\": \"+123456789\",\n"
    "      \"type\": \"WORK\"\n"
    "    },\n"
    "    {\n"
    "      \"number\": \"+987654321\",\n"
    "      \"type\": \"MOBILE\"\n"
    "    },\n"
    "    {\n"
    "      \"number\": \"+555555555\"\n"
    "    }\n"
    "  ]\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__person__descriptor, &protobuf_message, NULL, 0);
  ASSERT(result == 0);

  Foo__Person *person = (Foo__Person *)protobuf_message;

  //asm volatile ("int $3");

  printf("person->id: %d\n", person->id);
  printf("person->name: %s\n", person->name);
  printf("person->n_phone: %zu\n", person->n_phone);
  printf("person->phone[0]->number: %s\n", person->phone[0]->number);
  printf("person->phone[0]->type: %d\n", person->phone[0]->type);
  printf("person->phone[1]->number: %s\n", person->phone[1]->number);
  printf("person->phone[1]->type: %d\n", person->phone[1]->type);
  printf("person->phone[2]->number: %s\n", person->phone[2]->number);
  printf("person->phone[2]->type: %d\n", person->phone[2]->type);

  ASSERT(person->id == 42);
  ASSERT_STRCMP(person->name, "John Doe");

  char *json_string;
  result = protobuf2json_string(protobuf_message, JSON_INDENT(2), &json_string);
  ASSERT(result == 0);
  ASSERT(json_string);

  const char *expected_json_string = \
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42,\n"
    "  \"phone\": [\n"
    "    {\n"
    "      \"number\": \"+123456789\",\n"
    "      \"type\": \"WORK\"\n"
    "    },\n"
    "    {\n"
    "      \"number\": \"+987654321\",\n"
    "      \"type\": \"MOBILE\"\n"
    "    },\n"
    "    {\n"
    "      \"number\": \"+555555555\",\n"
    "      \"type\": \"HOME\"\n"
    "    }\n"
    "  ]\n"
    "}"
  ;

  ASSERT_STRCMP(
    json_string,
    expected_json_string
  );

  return 0;
}

int list__bad_json_string(void) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = "...";

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__list__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT(result == PROTOBUF2JSON_ERR_CANNOT_PARSE_STRING);

  const char *expected_error_string = \
    "JSON parsing error at line 1 column 1 (position 1): "
    "'[' or '{' expected near '.'"
  ;

  printf("Error: %s\n", error_string);

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  return 0;
}

int person__error_is_not_array(void) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"phone\": {}\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  //asm volatile ("int $3");

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__person__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT(result == PROTOBUF2JSON_ERR_IS_NOT_ARRAY);

  const char *expected_error_string = \
    "JSON is not an array required for repeatable GPB field"
  ;

  printf("Error: %s\n", error_string);

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  return 0;
}

int main(int argc, char **argv) {
  person__error_unknown_enum_value();

  list();

  person();

  list__bad_json_string();

  person__error_is_not_array();
}
