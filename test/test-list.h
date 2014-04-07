/*
 * Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

TEST_DECLARE(protobuf2json_string__person__required)
TEST_DECLARE(protobuf2json_string__person__optional)
TEST_DECLARE(protobuf2json_string__person__repeated_message)
TEST_DECLARE(protobuf2json_string__bar__default_values)

TEST_DECLARE(json2protobuf_string__person__required)
TEST_DECLARE(json2protobuf_string__person__optional)
TEST_DECLARE(json2protobuf_string__person__repeated_message)
TEST_DECLARE(json2protobuf_string__person__unknown_field)
TEST_DECLARE(json2protobuf_string__bar__default_values)
TEST_DECLARE(json2protobuf_string__bar__bad_json_string)


TASK_LIST_START
  TEST_ENTRY(protobuf2json_string__person__required)
  TEST_ENTRY(protobuf2json_string__person__optional)
  TEST_ENTRY(protobuf2json_string__person__repeated_message)
  TEST_ENTRY(protobuf2json_string__bar__default_values)

  TEST_ENTRY(json2protobuf_string__person__required)
  TEST_ENTRY(json2protobuf_string__person__optional)
  TEST_ENTRY(json2protobuf_string__person__repeated_message)
  TEST_ENTRY(json2protobuf_string__person__unknown_field)
  TEST_ENTRY(json2protobuf_string__bar__default_values)
  TEST_ENTRY(json2protobuf_string__bar__bad_json_string)
TASK_LIST_END
