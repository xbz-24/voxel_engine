		{
			SetOpen(window, settings, false);
		}
		else if (settings.editor.selected_settings_menu_option == ve::ui::SettingsMenuOption::Quit)
		{
			window.Close();
		}
		else
		{
			ApplyAdjustment(window, settings, 1);
		}
	}
}
