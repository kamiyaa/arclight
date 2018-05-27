#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <dirent.h>

static const char *paths[] = {
	"/sys/class/backlight",
	"/sys/class/leds"
};
static const unsigned int num_paths = sizeof(paths)/sizeof(char *);

void list_ifaces()
{
	DIR *fp;
	struct dirent *dir_entry;
	for (unsigned int i = 0; i < num_paths; i++) {
		if ((fp = opendir(paths[i])) == NULL)
			continue;

		while ((dir_entry = readdir(fp))) {
			if (strcmp(dir_entry->d_name, ".") == 0 ||
				strcmp(dir_entry->d_name, "..") == 0)
				continue;
			printf("%s\n", dir_entry->d_name);
		}
		closedir(fp);
	}
}

void set_brightness(int brightness)
{
	FILE *fp;
	if ((fp = fopen("brightness", "w")) == NULL) {
		perror("brightness");
		return;
	}

	fprintf(fp, "%d", brightness);
	fclose(fp);
}

void offset_brightness(int change)
{
	static const char maxfile[] = "max_brightness";
	static const char bfile[] = "brightness";
	int brightness, max_brightness;
	FILE *fp;

	if ((fp = fopen(maxfile, "r")) == NULL) {
		perror(maxfile);
		return;
	}
	if (fscanf(fp, "%d", &max_brightness) < 1) {
		fclose(fp);
		return;
	}
	fclose(fp);

	if ((fp = fopen(bfile, "r")) == NULL) {
		perror(bfile);
		return;
	}
	if (fscanf(fp, "%d", &brightness) < 1) {
		fclose(fp);
		return;
	}
	fclose(fp);

	brightness += max_brightness * change / 100;

	if (brightness > max_brightness)
		brightness = max_brightness;
	if (brightness < 0)
		brightness = 0;

	set_brightness(brightness);
}

int main(int argc, char **argv)
{
	if (argc == 1) {
		list_ifaces();
		return 0;
	}

	extern char *optarg;
	int c, change = 0;
	char s = 0;

	char *interface = NULL;
	while ((c = getopt(argc, argv, "u:d:i:s:")) != -1) {
		switch (c) {
		case 'u':
			change = atoi(optarg);
			break;
		case 'd':
			change = -1 * atoi(optarg);
			break;
		case 'i':
			interface = optarg;
			break;
		case 's':
			change = atoi(optarg);
			s = 1;
			break;
		default:
			return 1;
		}
	}

	if (interface == NULL || change == 0) {
		fprintf(stderr, "usage: %s [-i interface -ud offset]\n", argv[0]);
		return 1;
	}

	DIR *fp;
	struct dirent *dir_entry;
	for (unsigned int i = 0; i < num_paths; i++) {
		if ((fp = opendir(paths[i])) == NULL) {
			perror(paths[i]);
			continue;
		}

		while ((dir_entry = readdir(fp))) {
			if (strcmp(dir_entry->d_name, interface) == 0) {
				char dir_path[strlen(paths[i]) + strlen(dir_entry->d_name) + 2];
				strcpy(dir_path, paths[i]);
				strcat(dir_path, "/");
				strcat(dir_path, dir_entry->d_name);

				if (chdir(dir_path) == -1) {
					perror(dir_path);
					return 1;
				}
				if (s == 1)
					set_brightness(change);
				else
					offset_brightness(change);
				return 0;
			}
		}
	}

	return 0;
}
