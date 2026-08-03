
		/** Disables the debug overlay. */
		EngineConfig& HideDebugOverlay() noexcept;

		/** Enables or disables the in-game settings menu. */
		EngineConfig& EnableSettingsMenu(bool enabled = true) noexcept;

		/** Disables the in-game settings menu. */
		EngineConfig& DisableSettingsMenu() noexcept;

		/** Installs a callback invoked once per frame before commands are applied. */
		EngineConfig& OnUpdate(UpdateCallback callback) noexcept;

		/** Installs a callback that receives runtime diagnostics snapshots. */
		EngineConfig& OnDiagnostics(DiagnosticsCallback callback) noexcept;

		/** Installs a callback that receives runtime log lines. */
		EngineConfig& OnLog(LogCallback callback) noexcept;
	};
}
