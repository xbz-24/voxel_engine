		/** @param point Point to test. @return True when the point is inside the box. */
		[[nodiscard]] bool Contains(const glm::vec3& point) const noexcept;

		/** @return True when minimum and maximum describe a valid box. */
		[[nodiscard]] bool IsValid() const noexcept;

		/** @return Center point of the bounds. */
		[[nodiscard]] glm::vec3 Center() const noexcept;

		/** @return Size of the bounds along each axis. */
		[[nodiscard]] glm::vec3 Size() const noexcept;
	};
}
