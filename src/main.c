#include <stdio.h>

#include <jason/json.h>
#include <jason/serializer.h>
#include <jason/deserializer.h>

#include "fs.h"

int main(int argc, char** argv)
{
	char* json_text = 0;
	read_file(argv[1], &json_text);

	json_value_t json = json_deserialize(json_text);

	char* serialized_buf = json_serialize(&json);
	printf("Serialized buf: \n%s\n", serialized_buf);

	json_free(json);
	free(json_text);
	free(serialized_buf);
	return 0;
}

