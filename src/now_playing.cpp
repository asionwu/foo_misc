#include "stdafx.h"


namespace {
	static const GUID id_group = { 0xe27c7c95, 0x6ed4, 0x43df, { 0xa3, 0xd8, 0xa7, 0x57, 0x5d, 0x07, 0x94, 0x5d } };

	static mainmenu_group_popup_factory g_mainmenu_group(id_group, mainmenu_groups::playback, mainmenu_commands::sort_priority_dontcare, "Now playing");

	class _commands : public mainmenu_commands {
	public:
		enum {
			cmd_pl_rem = 0,
			cmd_recycle,
			cmd_cp_name,
			cmd_search_song,
			cmd_remove_album_md,
			cmd_total
		};

		t_uint32 get_command_count() override {
			return cmd_total;
		}

		GUID get_command(t_uint32 p_index) override {
			static const GUID id_pl_rem = { 0x674c0096, 0xb9fb, 0x4f4d, { 0xa0, 0xf6, 0x93, 0x7b, 0x35, 0x3f, 0x99, 0xaf } };
			static const GUID id_recycle = { 0x94eb3400, 0x0f40, 0x4c18, { 0xa8, 0x1b, 0x2b, 0xf2, 0x7d, 0xd0, 0xca, 0xa9 } };
			static const GUID id_cp_name = { 0x600677dc, 0xd2b8, 0x4444, { 0xa2, 0xb9, 0x71, 0xdf, 0xcd, 0x85, 0x30, 0x4e } };
			static const GUID id_search_song = { 0x6539b68e, 0x947, 0x4290, { 0xae, 0x18, 0xc, 0x60, 0x65, 0xc8, 0x2b, 0x3e } };
			static const GUID id_remove_album_md = { 0xd7395987, 0xf8b2, 0x4ea2, { 0xb4, 0x17, 0xe9, 0x91, 0x6, 0xc2, 0x2a, 0x58 } };

			switch (p_index) {
			case cmd_pl_rem: return id_pl_rem;
			case cmd_recycle: return id_recycle;
			case cmd_cp_name: return id_cp_name;
			case cmd_search_song: return id_search_song;
			case cmd_remove_album_md: return id_remove_album_md;
			default: uBugCheck();
			}
		}

		void get_name(t_uint32 p_index, pfc::string_base& p_out) override {
			switch (p_index) {
			case cmd_pl_rem: p_out = "Remove from playlist"; break;
			case cmd_recycle: p_out = "Recycle file"; break;
			case cmd_cp_name: p_out = "Copy song name"; break;
			case cmd_search_song: p_out = "Search for similar songs"; break;
			case cmd_remove_album_md: p_out = "Remove album metadata"; break;
			default: uBugCheck();
			}
		}

		bool get_description(t_uint32 p_index, pfc::string_base& p_out) override {
			switch (p_index) {
			case cmd_pl_rem: p_out = "Removes the currently playing song from its playlist."; return true;
			case cmd_recycle: p_out = "Moves the currently playing song to the trash bin and advances playback."; return true;
			case cmd_cp_name: p_out = "Copies the currently playing song name to the clipboard."; return true;
			case cmd_search_song: p_out = "Searches the library for similar songs."; return true;
			case cmd_remove_album_md: p_out = "Removes album metadata from the currently playing song."; return true;
			default: return false;
			}
		}

		GUID get_parent() override {
			return id_group;
		}

		void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) override {
			switch (p_index) {
			case cmd_pl_rem: exec_pl_rem(); break;
			case cmd_recycle: exec_recycle(); break;
			case cmd_cp_name:exec_cp_name(); break;
			case cmd_search_song: exec_search_song(); break;
			case cmd_remove_album_md: exec_remove_album_md(); break;
			default:
				uBugCheck();
			}
		}

		void exec_pl_rem() {
			auto pm = playlist_manager_v6::get();
			t_size pl_idx, item_idx;
			if (!pm->get_playing_item_location(&pl_idx, &item_idx)) {
				return;
			}

			bit_array_one arr(item_idx);
			pm->playlist_undo_backup(pl_idx);
			pm->playlist_remove_items(pl_idx, arr);
		}

		void exec_recycle() {
			auto pc = playback_control::get();
			auto pm = playlist_manager::get();

			metadb_handle_ptr item;
			if (!pc->get_now_playing(item)) {
				return;
			}

			t_size pl_idx, item_idx;
			bool found = pm->get_playing_item_location(&pl_idx, &item_idx);

			pc->stop();

			if (!recycle(fb2k::formatTrackTitle(item, "%path%"))) {
				return;
			}

			pc->next();

			if (found) {
				bit_array_one arr(item_idx);
				pm->playlist_remove_items(pl_idx, arr);
			}
		}

		bool recycle(const char* path) {
			std::string str(path);

			CA2W ca2w(str.c_str());
			std::wstring wstr = ca2w;

			wstr += std::wstring(1, L'\0');

			SHFILEOPSTRUCT fileOp = { 0 };
			fileOp.wFunc = FO_DELETE;
			fileOp.pFrom = wstr.c_str();
			fileOp.fFlags = FOF_ALLOWUNDO;

			int code = SHFileOperation(&fileOp);
			if (code != 0) {
				popup_message::g_complain(pfc::format("Could not recycle ", wstr.c_str(), "\nerror code ", code));
				return false;
			}

			// also recycle empty parent dir
			auto fs = filesystem::get(path);

			pfc::string8 parent;
			if (fs->get_parent_path(path, parent) && filesystem::g_is_empty_directory(parent, fb2k::noAbort)) {
				return recycle(parent);
			}

			return true;
		}

		void exec_cp_name() {
			metadb_handle_ptr item;
			if (!playback_control_v3::get()->get_now_playing(item)) {
				return;
			}

			set_clipboard(fb2k::formatTrackTitle(item, "[%artist% ][%title%]"));
		}

		void exec_search_song() {
			pfc::string_formatter query;

			metadb_handle_ptr item;
			if (playback_control_v3::get()->get_now_playing(item)) {
				auto info = &item->get_full_info_ref(fb2k::noAbort)->info();
				query << fb2k::formatTrackTitle(item, "[%artist% ][%title%]");
			}

			library_search_ui::get()->show(query.lowerCase());
		}

		void exec_remove_album_md() {
			metadb_handle_ptr item;
			if (!playback_control_v3::get()->get_now_playing(item)) {
				return;
			}

			clear_metadata(pfc::list_single_ref_t<metadb_handle_ptr>(item), fb2k::service_new<clear_album_filter>());
		}
	};

	static mainmenu_commands_factory_t<_commands> g_mainmenu_commands_factory;
}