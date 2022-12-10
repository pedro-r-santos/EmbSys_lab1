#include "embsys_lab1_socket.h"

extern int stderr_print(char *str)
{
	int return_value = fprintf(stderr, "%s", str);

	if (return_value < 0) {
		strcat(str, ": fprintf() ");
		perror(str);
                return EXIT_FAILURE;
	}

        return EXIT_SUCCESS;
}

extern int stdout_print(char *str)
{
        int return_value = printf("%s", str);

        if (return_value < 0) {
		strcat(str, ": printf() ");
		perror(str);
                return EXIT_FAILURE;
        }

        return EXIT_FAILURE;
}
