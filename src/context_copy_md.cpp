#include "stdafx.h"


namespace {
	static const GUID id_copy_md_group = { 0xa4180e66, 0x8a01, 0x41ea, { 0x81, 0xb7, 0xb3, 0x10, 0x43, 0xcc, 0x9d, 0x24 } };

	static contextmenu_group_popup_factory copy_md_group_factory(id_copy_md_group, id_context_menu_group, "Copy metadata", 1);

	class copy_md_items : public contextmenu_item_simple {
	public:
		enum {
			cmd_song = 0,
			cmd_title,
			cmd_artist,
			cmd_album,
			cmd_artist_album,
			cmd_folder,
			cmd_total
		};

		GUID get_parent() override {
			return id_copy_md_group;
		}

		unsigned get_num_items() override {
			return cmd_total;
		}

		void get_item_name(unsigned p_index, pfc::string_base& p_out) override {
			switch (p_index) {
			case cmd_song: p_out = "Song"; break;
			case cmd_title: p_out = "Title"; break;
			case cmd_artist: p_out = "Artist"; break;
			case cmd_album: p_out = "Album"; break;
			case cmd_artist_album: p_out = "Artist Album"; break;
			case cmd_folder: p_out = "Folder"; break;
			default: uBugCheck();
			}
		}

		GUID get_item_guid(unsigned p_index) override {
			static const GUID id_cmd_song = { 0x3bc114ac, 0x207d, 0x4a3c, { 0xa1, 0x6e, 0xfe, 0xe0, 0x62, 0x39, 0xb1, 0x6a } };
			static const GUID id_cmd_title = { 0x3475a715, 0x7cb3, 0x46ee, { 0xb3, 0x67, 0xef, 0x12, 0xd5, 0x99, 0x17, 0x26 } };
			static const GUID id_cmd_artist = { 0xaa40d55c, 0xb9ff, 0x417f, { 0xaa, 0xae, 0xe, 0x73, 0x9d, 0x88, 0x23, 0xbd } };
			static const GUID id_cmd_album = { 0x2e21e5fd, 0xf5d1, 0x40c4, { 0x81, 0x70, 0xe2, 0x80, 0x4f, 0xf9, 0x18, 0xdd } };
			static const GUID id_cmd_artist_album = { 0x6798346d, 0xdf7f, 0x4d4a, { 0x8b, 0x20, 0xea, 0xd3, 0x69, 0xb2, 0xb2, 0xe3 } };
			static const GUID id_cmd_folder = { 0x7484b70f, 0x90c1, 0x453c, { 0x80, 0x77, 0xbf, 0x9a, 0xb0, 0x51, 0x4f, 0xba } };

			switch (p_index) {
			case cmd_song: return id_cmd_song;
			case cmd_title: return id_cmd_title;
			case cmd_artist: return id_cmd_artist;
			case cmd_album: return id_cmd_album;
			case cmd_artist_album: return id_cmd_artist_album;
			case cmd_folder: return id_cmd_folder;
			default: uBugCheck();
			}
		}

		bool get_item_description(unsigned p_index, pfc::string_base& p_out) override {
			switch (p_index) {
			case cmd_song: p_out = "Copies the song name."; return true;
			case cmd_title: p_out = "Copies the title."; return true;
			case cmd_artist: p_out = "Copies the artist name."; return true;
			case cmd_album: p_out = "Copies the album name."; return true;
			case cmd_artist_album: p_out = "Copies artist & album names."; return true;
			case cmd_folder: p_out = "Copies the folder name."; return true;
			default:
				uBugCheck();
			}
		}

		void context_command(unsigned p_index, metadb_handle_list_cref p_data, const GUID& p_caller) override {
			switch (p_index) {
			case cmd_song: titleformatCopy(p_data, "[[%artist%] [%title%]]"); break;
			case cmd_title: titleformatCopy(p_data, "[%title%]"); break;
			case cmd_artist: titleformatCopy(p_data, "[%artist%]"); break;
			case cmd_album: titleformatCopy(p_data, "[%album%]"); break;
			case cmd_artist_album: titleformatCopy(p_data, "[[%artist%] [%album%]]"); break;
			case cmd_folder: titleformatCopy(p_data, "$directory_path(%path%)"); break;
			default:
				uBugCheck();
			}
		}

		void titleformatCopy(metadb_handle_list_cref p_data, const char* script) {
			auto dedups = extract_title_format(p_data, script);

			pfc::string_formatter data;
			for (auto i = dedups.first(); i.is_valid(); ++i) {
				if (!data.is_empty()) {
					data << "\n";
				}
				data << *i;
			}

			set_clipboard(data);
		};
	};

	static contextmenu_item_factory_t<copy_md_items> copy_md_items_factory;
}
