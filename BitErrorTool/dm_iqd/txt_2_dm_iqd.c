#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BIT_LEN 8

int main(int argc, char *argv[]) {

  FILE *std_format = fopen("./cmgjpjpap.dm_iqd", "rb");
  char *std_format_char = malloc(sizeof(char) * 89);
  fread(std_format_char, sizeof(char), 89, std_format);

  FILE *fp = fopen("./input_data.txt", "rb");
  fseek(fp, 0, SEEK_END);
  int pos = ftell(fp) - 1;
  fseek(fp, 0, SEEK_SET);
  char *input_data = malloc(sizeof(char) * pos);
  fread(input_data, sizeof(char), pos, fp);
  printf("输入比特为: %s\n", input_data);

  char *output_data = malloc(sizeof(char) * (pos / 8));
  memset(output_data, 0, pos / 8);
  int count = 0;
  // printf("%d\n", pos);
  for (int i = 0; i < pos / 8; i++) {
    for (int j = 0; j < BIT_LEN; j++) {
      // printf("当前字符：%c\n", input_data[j + i * 8]);
      if (input_data[j + i * 8] == '1') {
        output_data[i] = output_data[i] | 0b1;
      }
      output_data[i] = output_data[i] << 1;
    }
    printf("%#08b\n", output_data[i]);
  }

  char *output_data2 = strcat(std_format_char, output_data);

  FILE *output_fp = fopen("./output_data.txt", "wb");
  fwrite(output_data2, sizeof(char), 89 + pos / 8, output_fp);

  fclose(output_fp);
  fclose(fp);
  fclose(std_format);

  // free(output_data);
  // free(std_format_char);
  // free(input_data);
  // free(output_datw2);

  return EXIT_SUCCESS;
}
