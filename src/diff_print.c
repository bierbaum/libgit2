#include "diff_patch.h"
	git_diff *diff;
	int oid_strlen;
	git_diff_line line;
	unsigned int
		content_loaded : 1,
		content_allocated : 1;
	git_diff_file_content *ofile;
	git_diff_file_content *nfile;
	if (!pi->oid_strlen) {
			pi->oid_strlen = GIT_ABBREV_DEFAULT;
		else if (git_repository__cvar(&pi->oid_strlen, repo, GIT_CVAR_ABBREV) < 0)
	pi->oid_strlen += 1; /* for NUL byte */

	if (pi->oid_strlen > GIT_OID_HEXSZ + 1)
		pi->oid_strlen = GIT_OID_HEXSZ + 1;
	pi->diff = diff;

		pi->oid_strlen = diff->opts.id_abbrev;
	git_repository *repo;

	repo = patch->diff ? patch->diff->repo : NULL;

	pi->diff = patch->diff;

	pi->oid_strlen = patch->diff_opts.id_abbrev;

	pi->content_loaded = 1;
	pi->ofile = &patch->ofile;
	pi->nfile = &patch->nfile;
	return diff_print_info_init__common(pi, out, repo, format, cb, payload);
	int (*strcomp)(const char *, const char *) =
		pi->diff ? pi->diff->strcomp : git__strcmp;
	git_oid_tostr(start_oid, pi->oid_strlen, &delta->old_file.id);
	git_oid_tostr(end_oid, pi->oid_strlen, &delta->new_file.id);
		out, (pi->oid_strlen <= GIT_OID_HEXSZ) ?
	git_buf *out, const git_diff_delta *delta, int oid_strlen)
	git_oid_tostr(start_oid, oid_strlen, &delta->old_file.id);
	git_oid_tostr(end_oid, oid_strlen, &delta->new_file.id);
	/* TODO: Match git diff more closely */
		if (delta->old_file.mode == 0) {
		} else if (delta->new_file.mode == 0) {
		} else {
			git_buf_printf(out, "old mode %o\n", delta->old_file.mode);
			git_buf_printf(out, "new mode %o\n", delta->new_file.mode);
		}
	const char *oldpfx,
	const char *newpfx,
	const char *template)
	const char *oldpath = delta->old_file.path;
	const char *newpath = delta->new_file.path;
	if (git_oid_iszero(&delta->old_file.id)) {
		oldpfx = "";
		oldpath = "/dev/null";
	}
	if (git_oid_iszero(&delta->new_file.id)) {
		newpfx = "";
	return git_buf_printf(out, template, oldpfx, oldpath, newpfx, newpath);
	int oid_strlen)
	if (!oid_strlen)
		oid_strlen = GIT_ABBREV_DEFAULT + 1;
	git_buf_printf(out, "diff --git %s%s %s%s\n",
		oldpfx, delta->old_file.path, newpfx, delta->new_file.path);
	GITERR_CHECK_ERROR(diff_print_oid_range(out, delta, oid_strlen));
	if ((delta->flags & GIT_DIFF_FLAG_BINARY) == 0)
		diff_delta_format_with_paths(
			out, delta, oldpfx, newpfx, "--- %s%s\n+++ %s%s\n");
	return git_buf_oom(out) ? -1 : 0;
static int diff_print_load_content(
	diff_print_info *pi,
	git_diff_delta *delta)
	git_diff_file_content *ofile, *nfile;
	assert(pi->diff);
	ofile = git__calloc(1, sizeof(git_diff_file_content));
	nfile = git__calloc(1, sizeof(git_diff_file_content));
	GITERR_CHECK_ALLOC(ofile);
	GITERR_CHECK_ALLOC(nfile);
	if ((error = git_diff_file_content__init_from_diff(
			ofile, pi->diff, delta, true)) < 0 ||
		(error = git_diff_file_content__init_from_diff(
			nfile, pi->diff, delta, true)) < 0) {
		git__free(ofile);
		git__free(nfile);
		return error;
	}

	pi->content_loaded = 1;
	pi->content_allocated = 1;
	pi->ofile = ofile;
	pi->nfile = nfile;

	return 0;
		goto noshow;
	if (!pi->content_loaded &&
		(error = diff_print_load_content(pi, delta)) < 0)
		return error;
			goto noshow;

noshow:
	pi->line.num_lines = 1;
	return diff_delta_format_with_paths(
		pi->buf, delta, old_pfx, new_pfx,
		"Binary files %s%s and %s%s differ\n");
		pi->diff ? pi->diff->opts.old_prefix : DIFF_OLD_PREFIX_DEFAULT;
		pi->diff ? pi->diff->opts.new_prefix : DIFF_NEW_PREFIX_DEFAULT;
	int oid_strlen = binary && show_binary ?
		GIT_OID_HEXSZ + 1 : pi->oid_strlen;
			pi->buf, delta, oldpfx, newpfx, oid_strlen)) < 0)
		pi->diff ? pi->diff->opts.old_prefix : DIFF_OLD_PREFIX_DEFAULT;
		pi->diff ? pi->diff->opts.new_prefix : DIFF_NEW_PREFIX_DEFAULT;
	git__free(pi.nfile);
	git__free(pi.ofile);

/* print a git_patch to an output callback */
int git_patch_print(
	git_patch *patch,
	git_diff_line_cb print_cb,
	void *payload)
{
	int error;
	git_buf temp = GIT_BUF_INIT;
	diff_print_info pi;

	assert(patch && print_cb);

	if (!(error = diff_print_info_init_frompatch(
			&pi, &temp, patch,
			GIT_DIFF_FORMAT_PATCH, print_cb, payload)))
	{
		error = git_patch__invoke_callbacks(
			patch, diff_print_patch_file, diff_print_patch_binary,
			diff_print_patch_hunk, diff_print_patch_line, &pi);

		if (error) /* make sure error message is set */
			giterr_set_after_callback_function(error, "git_patch_print");
	}

	git_buf_free(&temp);

	return error;
}
