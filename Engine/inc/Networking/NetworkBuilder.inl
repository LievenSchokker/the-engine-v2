#pragma once

template <typename T, typename... Args>
void NetworkBuilder::command(const std::string& name, void (T::*method)(Args...))
{
	static_assert(std::is_base_of_v<NetworkBehaviour, T>, "T must derive from NetworkBehaviour");
	T* self = static_cast<T*>(&target);  // Changed from behaviour to target

	command(name, [self, method](ReadArchive& archive) {
		std::tuple<std::remove_cvref_t<Args>...> args;

		std::apply([&archive](auto&... a) {
			(archive.process(a), ...);
		}, args);

		std::apply([self, method](auto&... a) {
			(self->*method)(a...);
		}, args);
	});
}