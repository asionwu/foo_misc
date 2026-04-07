#include "stdafx.h"


namespace {
	static const GUID id_clear_md_group = { 0xb38a7fe, 0x135a, 0x4d6b, { 0xb4, 0x40, 0xf7, 0xd6, 0x82, 0x3d, 0x7b, 0x1c } };

	static contextmenu_group_popup_factory clear_md_group_factory(id_clear_md_group, id_context_menu_group, "Clear metadata", 1);

	class clear_all_filter : public file_info_filter {
	public:
		bool apply_filter(metadb_handle_ptr p_location, t_filestats p_stats, file_info& p_info) {
			auto artist = fb2k::formatTrackTitle(p_location, "[%artist%]");
			auto title = fb2k::formatTrackTitle(p_location, "[%title%]");

			p_info.meta_remove_all();
			p_info.meta_set("artist", artist);
			p_info.meta_set("title", title);

			return true;
		}
	};

	class clear_md_items : public contextmenu_item_simple {
	public:
		enum {
			cmd_album,
			cmd_all,
			cmd_total
		};

		GUID get_parent() override {
			return id_clear_md_group;
		}

		unsigned get_num_items() override {
			return cmd_total;
		}

		void get_item_name(unsigned p_index, pfc::string_base& p_out) override {
			switch (p_index) {
			case cmd_album: p_out = "Album"; break;
			case cmd_all: p_out = "All but song"; break;
			default: uBugCheck();
			}
		}

		GUID get_item_guid(unsigned p_index) override {
			static const GUID id_cmd_album = { 0xa4d3fde, 0xacce, 0x4233, { 0xa8, 0x8, 0x7c, 0xd4, 0xa4, 0xf1, 0x46, 0x11 } };
			static const GUID id_cmd_all = { 0x97af54df, 0xb733, 0x45fb, { 0x80, 0x92, 0x2a, 0x1f, 0xd5, 0xd, 0xba, 0xa4 } };

			switch (p_index) {
			case cmd_album: return id_cmd_album;
			case cmd_all: return id_cmd_all;
			default: uBugCheck();
			}
		}

		bool get_item_description(unsigned p_index, pfc::string_base& p_out) override {
			switch (p_index) {
			case cmd_album: p_out = "Clears album metadata."; return true;
			case cmd_all: p_out = "Clears all metadata but artists and title."; return true;
			default:
				uBugCheck();
			}
		}

		void context_command(unsigned p_index, metadb_handle_list_cref p_data, const GUID& p_caller) override {
			switch (p_index) {
			case cmd_album: clear_metadata(p_data, fb2k::service_new<clear_album_filter>()); break;
			case cmd_all: clear_metadata(p_data, fb2k::service_new<clear_all_filter>()); break;
			default:
				uBugCheck();
			}
		}
	};

	static contextmenu_item_factory_t<clear_md_items> clear_md_items_factory;
}
