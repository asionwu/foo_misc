#include "stdafx.h"


static contextmenu_group_factory group_factory(id_context_menu_group, contextmenu_groups::root, 0);

void set_clipboard(pfc::string in) {
	if (in.is_empty()) {
		return;
	}

	pfc::stringcvt::string_os_from_utf8 tmp(in);
	const size_t size = (tmp.length() + 1) * sizeof(TCHAR);

	HGLOBAL buffer = GlobalAlloc(GMEM_DDESHARE, size);
	if (buffer == NULL) throw std::bad_alloc();

	try {
		CGlobalLockScope lock(buffer);
		PFC_ASSERT(lock.GetSize() == size);
		memcpy(lock.GetPtr(), tmp, size);
	}
	catch (...) {
		GlobalFree(buffer); throw;
	}

	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, buffer);
	CloseClipboard();
}

pfc::avltree_t<pfc::string> extract_title_format(metadb_handle_list_cref p_data, const char* script) {
	pfc::avltree_t<pfc::string> out;

	for (t_size i = 0; i < p_data.get_count(); ++i) {
		auto fmt = fb2k::formatTrackTitle(p_data[i], script);
		if (!fmt.is_empty()) {
			out += fmt;
		}
	}

	return out;
}

void clear_metadata(metadb_handle_list_cref p_data, service_ptr_t<file_info_filter> p_filter) {
	metadb_io_v2::get()->update_info_async(p_data, p_filter, core_api::get_main_window(), metadb_io_v2::op_flag_silent, NULL);
};

bool clear_album_filter::apply_filter(metadb_handle_ptr p_location, t_filestats p_stats, file_info& p_info) {
	p_info.meta_remove_field("album");
	p_info.meta_remove_field("album artist");
	p_info.meta_remove_field("track");
	p_info.meta_remove_field("tracknumber");
	p_info.meta_remove_field("totaltracks");
	p_info.meta_remove_field("disc");
	p_info.meta_remove_field("discnumber");
	p_info.meta_remove_field("totaldiscs");
	return true;
}

pfc::string clean_up(pfc::string in) {
	in.replace_string("(", " ");
	in.replace_string(")", " ");

	pfc::string out;
	out.convert_to_lower_ascii(in, ' ');

	return out.toLower();
}