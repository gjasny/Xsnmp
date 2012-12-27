#include <regex.h>

void scale_value_to_m(char unit_char, uint32_t *val);
uint32_t extract_uint_in_range (char *start, size_t len);
uint32_t extract_uint_in_group(const char *start, regmatch_t group);
uint32_t extract_uint_from_regex (char *data, size_t data_len, char *expression);
void trim_end (char *str);
char* extract_string_in_range (char *start, size_t len, char **dest, size_t *dest_len);
char* extract_string_in_group(const char *start, regmatch_t group, char **dest, size_t *dest_len);
char* extract_string_from_regex (char *data, size_t data_len, char *expression, char **dest, size_t *dest_len);
void extract_U64_from_regex (char *data, size_t data_len, char *expression, U64 *val);
uint32_t scale_U64_to_K (U64 *val);
U64 sum_U64 (U64 x, U64 y);
int extract_boolean_from_regex (char *data, size_t data_len, char *expression);
void extract_U64_in_range (char *start, size_t len, U64 *val);
