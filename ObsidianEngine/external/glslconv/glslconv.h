#ifndef __GLSLCONV_H__
#define __GLSLCONV_H__

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct {
		char* vertex;
		char* fragment;
	} glslconv_output_t;

	glslconv_output_t glslconv_compile_330(char* data, const char* include_path);
	glslconv_output_t glslconv_compile_300_es(char* data, const char* include_path);

#ifdef __cplusplus
}
#endif

#endif

