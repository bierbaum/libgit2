#include "clar_libgit2.h"
#include "cache.h"

void test_core_opts__cleanup(void)
{
	cl_git_pass(git_libgit2_opts(GIT_OPT_SET_EXTENSIONS, NULL, 0));
}

void test_core_opts__readwrite(void)
{
	size_t old_val = 0;
	size_t new_val = 0;

	git_libgit2_opts(GIT_OPT_GET_MWINDOW_SIZE, &old_val);
	git_libgit2_opts(GIT_OPT_SET_MWINDOW_SIZE, (size_t)1234);
	git_libgit2_opts(GIT_OPT_GET_MWINDOW_SIZE, &new_val);

	cl_assert(new_val == 1234);

	git_libgit2_opts(GIT_OPT_SET_MWINDOW_SIZE, old_val);
	git_libgit2_opts(GIT_OPT_GET_MWINDOW_SIZE, &new_val);

	cl_assert(new_val == old_val);
}

void test_core_opts__invalid_option(void)
{
	cl_git_fail(git_libgit2_opts(-1, "foobar"));
}

void test_core_opts__extensions_query(void)
{
	git_strarray out = { 0 };

	cl_git_pass(git_libgit2_opts(GIT_OPT_GET_EXTENSIONS, &out));

	cl_assert(out.count > 0);
	cl_assert_equal_s("noop", out.strings[0]);

	git_strarray_dispose(&out);
}

void test_core_opts__extensions_add(void)
{
	const char *in[] = { "foo" };
	git_strarray out_before = { 0 };
	git_strarray out_after = { 0 };
	size_t idx = 0;

	cl_git_pass(git_libgit2_opts(GIT_OPT_GET_EXTENSIONS, &out_before));
	cl_git_pass(git_libgit2_opts(GIT_OPT_SET_EXTENSIONS, in, ARRAY_SIZE(in)));
	cl_git_pass(git_libgit2_opts(GIT_OPT_GET_EXTENSIONS, &out_after));

	cl_assert_equal_sz(out_after.count, out_before.count + 1);

	for(idx = 0; idx < out_before.count; idx++) {
	  cl_assert_equal_s(out_before.strings[idx], out_after.strings[idx]);
	}

	cl_assert_equal_s("foo", out_after.strings[out_after.count - 1]);

	git_strarray_dispose(&out_before);
	git_strarray_dispose(&out_after);
}

void test_core_opts__extensions_remove(void)
{
	const char *in[] = { "bar", "!negate", "!noop", "baz" };
	git_strarray out_before = { 0 };
	git_strarray out_after = { 0 };
	size_t idx = 0;

	bool found_bar = false;
	bool found_baz = false;

	cl_git_pass(git_libgit2_opts(GIT_OPT_GET_EXTENSIONS, &out_before));
	cl_git_pass(git_libgit2_opts(GIT_OPT_SET_EXTENSIONS, in, ARRAY_SIZE(in)));
	cl_git_pass(git_libgit2_opts(GIT_OPT_GET_EXTENSIONS, &out_after));


	for(idx = 0; idx < out_after.count; idx++) {
	  found_bar = found_bar || !strcmp(out_after.strings[idx], "bar");
	  found_baz = found_baz || !strcmp(out_after.strings[idx], "baz");
	  cl_assert(strcmp(out_after.strings[idx], "negate"));
	  cl_assert(strcmp(out_after.strings[idx], "noop"));
	}

	cl_assert(found_bar);
	cl_assert(found_baz);

	git_strarray_dispose(&out_before);
	git_strarray_dispose(&out_after);
}
