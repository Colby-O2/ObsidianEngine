#include "glslconv.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

// must be shorter then   "gl_FragColor"
#define FRAGMENT_OUT_NAME "_glslc_fout"

#define PRECISION_CODE    "precision mediump float;"
#define FRAGMENT_OUT_CODE "out vec4 " FRAGMENT_OUT_NAME ";"

typedef struct {
	char* data;
	int len;
} str_t;

typedef struct {
	char* inp;
	char* vert;
	char* frag;
	int stage;
	int line;
} comp_ctx_t;

enum {
	STAGE_COMMON,
	STAGE_VERTEX,
	STAGE_FRAGMENT,
};

static char* strrstr(char* st, const char* fnd);
static inline void compile_error(const char* msg);
static inline bool has_prefix(const char* str, const char* prefix);
static inline bool is_name_char(char c);
static char* is_at_function(char* data);
static inline bool is_name(const char* str, const char* name);
static void output_data(comp_ctx_t* ctx, int len);
static void output_string(comp_ctx_t* ctx, const char* str);
static const char* get_directory_path(const char* filepath);
static char* read_file(const char* filename, int name_length, const char* directory);
static char* resolve_includes(char* data, const char* include_path);
static glslconv_output_t glslconv_compile(char* data);

static inline void compile_error(const char* msg) {
	fprintf(stderr, "[glslconv] error: %s\n", msg);
	exit(1);
}

// https://cplusplus.com/forum/lounge/285814/
static char* strrstr(char* st, const char* fnd) {
	if (!*fnd)
		return st;

	char* stbeg = st;
	const char* fndbeg = fnd;

	for (; *st; ++st);
	for (; *fnd; ++fnd);

	const size_t fndlen = fnd - fndbeg;

	if (st - stbeg < fndlen)
		return NULL;

	st -= fndlen;

	for (int notgot = 1; st >= stbeg; --st) {
		for (const char* itr = st, *fit = fndbeg; fit != fnd && !(notgot = (*itr++ != *fit++)); );

		if (!notgot)
			return st;
	}

	return NULL;
}

static inline bool has_prefix(const char* str, const char* prefix) {
	while (*str && *prefix) {
		if (*str++ != *prefix++) return false;
	}
	if (*str == 0 && *prefix == 0) return true;
	if (*str == 0) return false;
	return true;
}

static inline bool is_name_char(char c) {
	return (
		(c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		(c >= '0' && c <= '9') ||
		c == '_'
	);
}

static char* is_at_function(char* data) {
	char* name_start = NULL;
	while (*data && (is_name_char(*data) || isspace(*data))) {
		if (!is_name_char(data[0]) && is_name_char(data[1])) {
			name_start = data + 1;
		}
		data += 1;
	}
	if (*data == '(') return name_start;
	return NULL;
}

static inline bool is_name(const char* str, const char* name) {
	return (
		has_prefix(str, name) &&
		!is_name_char(str[strlen(name)]) &&
		!is_name_char(str[-1])
	);
}

static void output_data(comp_ctx_t* ctx, int len) {
	if (ctx->stage == STAGE_COMMON) {
		for (int i = 0; i < len; i++) {
			if (*ctx->inp == '\n') ctx->line += 1;
			*ctx->vert++ = *ctx->inp;
			*ctx->frag++ = *ctx->inp;
			ctx->inp++;
		}
	} else if (ctx->stage == STAGE_VERTEX) {
		for (int i = 0; i < len; i++) {
			if (*ctx->inp == '\n') ctx->line += 1;
			*ctx->vert++ = *ctx->inp++;
		}
	} else if (ctx->stage == STAGE_FRAGMENT) {
		for (int i = 0; i < len; i++) {
			if (*ctx->inp == '\n') ctx->line += 1;
			*ctx->frag++ = *ctx->inp++;
		}
	}
}

static void output_string(comp_ctx_t* ctx, const char* str) {
	if (ctx->stage == STAGE_COMMON) {
		while (*str) {
			*ctx->vert++ = *str;
			*ctx->frag++ = *str;
			str++;
		}
	} else if (ctx->stage == STAGE_VERTEX) {
		while (*str) {
			*ctx->vert++ = *str++;
		}
	} else if (ctx->stage == STAGE_FRAGMENT) {
		while (*str) {
			*ctx->frag++ = *str++;
		}
	}
}

static glslconv_output_t glslconv_compile(char* data) {
	int data_len = strlen(data);
	char* vert_out = malloc(data_len * 2);
	char* frag_out = malloc(data_len * 2);
	comp_ctx_t ctx = {
		.inp = data,
		.vert = vert_out,
		.frag = frag_out,
		.stage = STAGE_COMMON,
		.line = 1,
	};

	static char tmp_buf[256];

	char* func_name;
	while (*ctx.inp) {
		if (has_prefix(ctx.inp, "#vertex")) {
			ctx.stage = STAGE_VERTEX;
			ctx.inp += strlen("#vertex");
			sprintf(tmp_buf, "%d", ctx.line+1);
			output_string(&ctx, "#line ");
			output_string(&ctx, tmp_buf);
			output_string(&ctx, "\n");
		} else if (has_prefix(ctx.inp, "#fragment")) {
			ctx.stage = STAGE_FRAGMENT;
			ctx.inp += strlen("#fragment");
			sprintf(tmp_buf, "%d", ctx.line+1);
			output_string(&ctx, "#line ");
			output_string(&ctx, tmp_buf);
			output_string(&ctx, "\n");
		} else if (has_prefix(ctx.inp, "#common")) {
			ctx.stage = STAGE_COMMON;
			ctx.inp += strlen("#common");
			sprintf(tmp_buf, "%d", ctx.line+1);
			output_string(&ctx, "#line ");
			output_string(&ctx, tmp_buf);
			output_string(&ctx, "\n");
		} else if (has_prefix(ctx.inp, "#version")) {
			int save = ctx.stage;
			char* end = strchr(ctx.inp, '\n');
			output_data(&ctx, end - ctx.inp + 1);
			ctx.stage = STAGE_FRAGMENT;
			output_string(&ctx, "precision mediump         float;");
			output_string(&ctx, "out vec4 ");
			output_string(&ctx, FRAGMENT_OUT_NAME);
			output_string(&ctx, ";");
			ctx.stage = save;
		} else if (has_prefix(ctx.inp, "\nvec4 Vertex(")) {
			while (*ctx.inp++ != '{');
			int bracket = 1;
			output_string(&ctx, "void main() {");
			while (bracket > 0) {
				if (*ctx.inp == '{') {
					bracket += 1;
					output_data(&ctx, 1);
				} else if (*ctx.inp == '}') {
					bracket -= 1;
					output_data(&ctx, 1);
				} else if (is_name(ctx.inp, "return")) {
					ctx.inp += strlen("return");
					output_string(&ctx, "gl_Position = ");
				} else {
					output_data(&ctx, 1);
				}
			}
		} else if (has_prefix(ctx.inp, "\nvec4 Fragment(")) {
			while (*ctx.inp++ != '{');
			int bracket = 1;
			output_string(&ctx, "void main() {");
			while (bracket > 0) {
				if (*ctx.inp == '{') {
					bracket += 1;
					output_data(&ctx, 1);
				} else if (*ctx.inp == '}') {
					bracket -= 1;
					output_data(&ctx, 1);
				} else if (is_name(ctx.inp, "return")) {
					ctx.inp += strlen("return");
					output_string(&ctx, "{");
					output_string(&ctx, FRAGMENT_OUT_NAME);
					output_string(&ctx, " = ");
					while (*ctx.inp != ';') {
						output_data(&ctx, 1);
					}
					output_data(&ctx, 1);
					output_string(&ctx, "return;}");
				} else {
					output_data(&ctx, 1);
				}
			}
		} else {
			output_data(&ctx, 1);
		}
	}

	*ctx.vert = 0;
	*ctx.frag = 0;

	char* ptr = vert_out;
	while (*ptr) {
		if (has_prefix(ptr, "varying")) {
			memset(ptr, ' ', strlen("varying"));
			memcpy(ptr, "out", 3);
		} else if (has_prefix(ptr, "attribute")) {
			memset(ptr, ' ', strlen("attribute"));
			memcpy(ptr, "in", 2);
		} else {
			ptr += 1;
		}
	}

	ptr = frag_out;
	while (*ptr) {
		if (has_prefix(ptr, "varying")) {
			memset(ptr, ' ', strlen("varying"));
			memcpy(ptr, "in", 2);
		} else if (has_prefix(ptr, "attribute")) {
			char* end = strchr(ptr, ';');
			memset(ptr, ' ', end - ptr + 1);
		} else {
			ptr += 1;
		}
	}

	return (glslconv_output_t){vert_out, frag_out};
}

static const char* get_directory_path(const char* filepath) {
	static char dir_path[256];
	dir_path[0] = 0;

	const char* last_slash = strrchr(filepath, '/');
	if (last_slash == NULL) {
		strcpy(dir_path, "./");
		return dir_path;
	}

	size_t dir_length = last_slash - filepath + 1;
	strncpy(dir_path, filepath, dir_length);
	dir_path[dir_length] = '\0';

	return dir_path;
}


static char* read_file(const char* filename, int name_length, const char* directory) {
	size_t path_length = strlen(directory) + name_length + 1;
	char* full_path = malloc(path_length);
	full_path[0] = 0;
	strcat(full_path, directory);
	strncat(full_path, filename, name_length);
	printf("include: %d %s\n", name_length, full_path);

	FILE* file = fopen(full_path, "r");
	free(full_path);
	if (!file) {
		fprintf(stderr, "Failed to open include file '%s'!\n", filename);
		exit(1);
	}

	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	rewind(file);

	char* data = (char*)malloc(file_size + 1);
	fread(data, 1, file_size, file);
	data[file_size] = '\0';
	fclose(file);
	return data;
}

static char* resolve_includes(char* data, const char* include_path) {
	int len = strlen(data);
	char* out = malloc(len + 1);
	int ptr = 0;

	while (*data) {
		if (has_prefix(data, "#include \"")) {
			char* name = data + strlen("#include \"");
			char* name_end = strchr(name, '"');
			data = name_end + 1;
			char* inc_data = read_file(name, name_end - name, include_path);
			char* res_inc_data = resolve_includes(inc_data, include_path);
			int res_inc_data_len = strlen(res_inc_data);
			free(inc_data);
			len += res_inc_data_len;
			char* new_out = realloc(out, len + 1);
			if (new_out == NULL) {
				fprintf(stderr, "Failed to realloc!\n");
				exit(1);
			}
			out = new_out;
			memcpy(&out[ptr], res_inc_data, res_inc_data_len);
			ptr += res_inc_data_len;
			free(res_inc_data);
			out[ptr++] = '\n';
		} else {
			out[ptr++] = *data++;
		}
	}

	out[ptr] = 0;
	return out;
}

glslconv_output_t glslconv_compile_330(char* data, const char* include_path) {
	char* resolved_data = resolve_includes(data, get_directory_path(include_path));

	glslconv_output_t out = glslconv_compile(resolved_data);
	free(resolved_data);

	char* version = strstr(out.vertex, "#version glslconv");
	version += strlen("#version ");
	memset(version, ' ', strlen("glslconv"));
	memcpy(version, "330", 3);
	version = strstr(out.fragment, "#version glslconv");
	version += strlen("#version ");
	memset(version, ' ', strlen("glslconv"));
	memcpy(version, "330", 3);

	char* preset_prec = strstr(out.fragment, "\nprecision ") + 1;
	char* preset_prec_slot = preset_prec + strlen("precision ");
	char* set_prec = strrstr(out.fragment, "\nprecision ") + 1;
	if (set_prec != NULL && set_prec != preset_prec) {
		char* mode = set_prec + strlen("precision ");
		char* end = mode;
		while (is_name_char(*end)) end += 1;
		int len = end - mode;
		memset(preset_prec_slot, ' ', 10);
		memcpy(preset_prec_slot, mode, len);

		char* line_end = strchr(set_prec, '\n');
		len = line_end - set_prec;
		memset(set_prec, ' ', len);
	};

	return out;
}

glslconv_output_t glslconv_compile_300_es(char* data, const char* include_path) {
	char* resolved_data = resolve_includes(data, get_directory_path(include_path));

	glslconv_output_t out = glslconv_compile(resolved_data);
	free(resolved_data);

	char* version = strstr(out.vertex, "#version glslconv");
	version += strlen("#version ");
	memset(version, ' ', strlen("glslconv"));
	memcpy(version, "300 es", 6);
	version = strstr(out.fragment, "#version glslconv");
	version += strlen("#version ");
	memset(version, ' ', strlen("glslconv"));
	memcpy(version, "300 es", 6);

	char* preset_prec = strstr(out.fragment, "\nprecision ") + 1;
	char* preset_prec_slot = preset_prec + strlen("precision ");
	while (true) {
		char* set_prec = strrstr(out.fragment, "\nprecision ") + 1;
		if (set_prec != NULL && set_prec != preset_prec) {
			char* mode = set_prec + strlen("precision ");
			char* end = mode;
			while (is_name_char(*end)) end += 1;
			int len = end - mode;
			memset(preset_prec_slot, ' ', 10);
			memcpy(preset_prec_slot, mode, len);

			char* line_end = strchr(set_prec, '\n');
			len = line_end - set_prec;
			memset(set_prec, ' ', len);
		} else break;
	}

	return out;
}

