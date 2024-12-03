#include <stdio.h>

#include <jason/json.h>
#include <jason/serializer.h>
#include <jason/deserializer.h>

#include "fs.h"

const char* JSON_DB_PATH = "db.json";

json_object_t* add_new_diff(json_object_t* parent, char* diff, char* name)
{
	char* copied_name = (char*)calloc(strlen(name) + 1, sizeof(char));
	memcpy(copied_name, name, strlen(name));

	json_value_t val = (json_value_t){.type = JSON_OBJECT, .value.obj = (json_object_t*)calloc(1, sizeof(json_object_t)) };
	json_object_t* next_obj = (json_object_t*)calloc(1, sizeof(json_object_t));

	json_obj_add_entry(val.value.obj, "yes", (json_value_t){.type = JSON_STRING, .value.str = copied_name});
	json_obj_add_entry(val.value.obj, "no", (json_value_t){.type = JSON_OBJECT, .value.obj = next_obj});

	json_obj_add_entry(parent, diff, val);
	return next_obj;
}

int main(int argc, char** argv)
{
	json_object_t* root = 0;
	json_value_t val = (json_value_t){0};

	char* json_text = 0;
	size_t len = read_file(JSON_DB_PATH, &json_text);
	if(!len)
	{
		printf("creating new file\n");
		root = (json_object_t*)calloc(1, sizeof(json_object_t));
		val = (json_value_t){.type = JSON_OBJECT, .value.obj = root};
	}
	else
	{
		printf("deserializing %s\n", json_text);
		val = json_deserialize(json_text);
		root = val.value.obj;
	}

	json_object_t* next = add_new_diff(root, "real", "amogus");
	next = add_new_diff(next, "red", "it is red");
	next = add_new_diff(next, "aa", "aa");
	next = add_new_diff(next, "bb", "bb");
	next = add_new_diff(next, "cc", "cc");
	next = add_new_diff(next, "dd", "dd");
	next = add_new_diff(next, "ee", "ee");

	json_obj_add_entry(next, "isfinal", (json_value_t){.type = JSON_TRUE});

	char* serialized = json_serialize(&val);
	write_file(JSON_DB_PATH, serialized, strlen(serialized));

	printf("%s\n", serialized);

	return 0;
}

