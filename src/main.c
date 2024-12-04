#include <stdbool.h>
#include <stdio.h>

#include <jason/json.h>
#include <jason/serializer.h>
#include <jason/deserializer.h>

#include "fs.h"
#include "graph.h"
#include "color.h"

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


void guess(json_value_t* val)
{
	if(!val) return;
	if(val->type == JSON_STRING)
	{
		printf("It is %s\n", val->value.str);
		return;
	}

	if(val->type != JSON_OBJECT)
		return;

	json_object_t* elem = val->value.obj;

	if(elem->elem_cnt == 0)
	{
		printf("i have no fukcing idea!\n");
		printf("what is it and how it is different?\n");
		char* item = (char*)calloc(256, sizeof(char));
		char* difference = (char*)calloc(256, sizeof(char));
		scanf("%255s", item);
		scanf("%255s", difference);

		json_object_t* next = add_new_diff(elem, item, difference);
		return;
	}

	while(1)
	{
		printf("Is it %s?\n", elem->elements[0].key);
		char str[256] = {0};
		scanf("%255s", (char*)str);

		if(strcasecmp(str, "yes") == 0)
		{
			guess(json_obj_get(elem->elements[0].value.value.obj, "yes"));
			return;
		}

		if(strcasecmp(str, "no") == 0)
		{
			guess(json_obj_get(elem->elements[0].value.value.obj, "no"));
			return;
		}

		printf("yes/no\n");
	}
}

typedef struct qualities
{
	char** strs;
	size_t cnt;
	size_t allocated;
} qualities_t;


static void print_key(json_char_t* key, qualities_t* qualities)
{
	if(!qualities) return;

	qualities->strs[qualities->cnt++] = key;

	if(qualities->cnt >= qualities->allocated)
	{
		qualities->allocated *= 2;
		qualities->strs = (char**)realloc(qualities->strs, qualities->allocated * sizeof(char*));
	}
}

static bool search(json_object_t* obj, char* to_find, char* prev, qualities_t* qualities)
{
	for(int i = 0; i < obj->elem_cnt; i++)
	{
		json_value_t elem = obj->elements[i].value;
		json_char_t* key = obj->elements[i].key;

		if(elem.type == JSON_OBJECT)
		{
			if(search(elem.value.obj, to_find, key, qualities))
			{
				print_key(key, qualities);
				return true;
			}
		}

		if(elem.type == JSON_STRING && strcmp(elem.value.str, to_find) == 0)
		{
			print_key(key, qualities);
			return true;
		}
	}

	return false;
}

void definition(json_object_t* root)
{
	printf("object?\n");

	char to_find[256] = {0};
	scanf("%255s", (char*)to_find);

	qualities_t qual = { .cnt = 0, .allocated = 16, .strs =  (char**)calloc(16, sizeof(char*))};

	if(!search(root, to_find, 0, &qual))
	{
		printf("Can't find this object!\n");
	}

	for(int i = qual.cnt - 1; i > 0; i -= 2)
	{
		char* key = qual.strs[i];
		char* state = qual.strs[i - 1];

		if(strcmp(state, "yes") == 0)
		{
			printf(GREEN "IS %s   " RESET, key);
		}
		else if(strcmp(state, "no") == 0)
		{
			printf(RED "NOT %s   " RESET, key);
		}
	}

	printf("\n");

	free(qual.strs);
}

void compare(json_object_t* root)
{
	char to_find1[256] = {0};
	char to_find2[256] = {0};
	printf("object 1?\n");
	scanf("%255s", (char*)to_find1);

	printf("object 2?\n");
	scanf("%255s", (char*)to_find2);

	qualities_t qual1 = { .cnt = 0, .allocated = 16, .strs =  (char**)calloc(16, sizeof(char*))};
	qualities_t qual2 = { .cnt = 0, .allocated = 16, .strs =  (char**)calloc(16, sizeof(char*))};

	if(!search(root, to_find1, 0, &qual1))
	{
		printf("Can't find object1!\n");
	}

	if(!search(root, to_find2, 0, &qual2))
	{
		printf("Can't find object2!\n");
	}


	for(int i = qual1.cnt - 1; i > 0; i -= 2)
	{
		char* key = qual1.strs[i];
		char* state = qual1.strs[i - 1];

		if(strcmp(state, "yes") == 0)
		{
			printf(GREEN "IS %s   " RESET, key);
		}
		else if(strcmp(state, "no") == 0)
		{
			printf(RED "NOT %s   " RESET, key);
		}
	}

	printf("\n");

	for(int i = qual2.cnt - 1; i > 0; i -= 2)
	{
		char* key = qual2.strs[i];
		char* state = qual2.strs[i - 1];

		if(strcmp(state, "yes") == 0)
		{
			printf(GREEN "IS %s   " RESET, key);
		}
		else if(strcmp(state, "no") == 0)
		{
			printf(RED "NOT %s   " RESET, key);
		}
	}

	printf("\n");

	free(qual1.strs);
	free(qual2.strs);
}



int main(int argc, char** argv)
{
	json_object_t* root = 0;
	json_value_t val = (json_value_t){0};
	char* json_text = 0;


	size_t len = read_file(JSON_DB_PATH, &json_text);

	if(!len)
	{
		root = (json_object_t*)calloc(1, sizeof(json_object_t));
		val = (json_value_t){.type = JSON_OBJECT, .value.obj = root};
	}
	else
	{
		val = json_deserialize(json_text);
		root = val.value.obj;
	}

	while(1)
	{
		printf("1 - guess 2 - definition 3 - compare 4 - draw graph 5 - quit\n");
		int res = 0;
		scanf("%d", &res);
		switch(res)
		{
			case 1:
				guess(&val);
				break;
			case 2:
				definition(root);
				break;
			case 3:
				compare(root);
				break;
			case 4:
				render_graph(&val, "amogus.png");
				system("imv amogus.png");
				break;
			case 5:
				goto ex;
				break;
			default:
				printf("unknown\n");
				break;
		}

		while ( getchar() != '\n' && getchar() != EOF )  
			;
	}

ex:
	char* serialized = json_serialize(&val);
	write_file(JSON_DB_PATH, serialized, strlen(serialized));

	return 0;
}

