#include <helpers/foobar2000+atl.h>
#include <libPPUI/clipboard.h>


static const GUID id_context_menu_group = { 0xecfaa596, 0x6882, 0x4285, { 0x9c, 0x75, 0x5c, 0xb8, 0x82, 0xb0, 0x8a, 0x84 } };

void set_clipboard(pfc::string text);

pfc::avltree_t<pfc::string> extract_title_format(metadb_handle_list_cref p_data, const char* script);

void clear_metadata(metadb_handle_list_cref p_data, service_ptr_t<file_info_filter> p_filter);

class clear_album_filter : public file_info_filter {
public:
	bool apply_filter(metadb_handle_ptr p_location, t_filestats p_stats, file_info& p_info);
};

pfc::string clean_up(pfc::string in);