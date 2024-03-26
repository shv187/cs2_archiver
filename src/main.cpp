#include <expected>
#include <filesystem>
#include <fstream>
#include <print>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

namespace archiver {
namespace windows {
namespace registry {
auto get_string(HKEY key, PCWSTR sub_key, PCWSTR value) noexcept
    -> std::expected<std::wstring, std::string> {
  DWORD key_type{ 0 };
  DWORD data_size{ 0 };
  constexpr DWORD flags{ RRF_RT_REG_SZ };
  auto result =
      ::RegGetValue(key, sub_key, value, flags, &key_type, nullptr, &data_size);
  if (result != ERROR_SUCCESS) {
    return std::unexpected("Couldn't get string length");
  }

  if (key_type != REG_SZ) {
    return std::unexpected("Wrong value type");
  }

  std::wstring text{};
  text.reserve(data_size / sizeof(WCHAR));

  result = ::RegGetValue(key, sub_key, value, flags, nullptr,
                         reinterpret_cast<void*>(text.data()), &data_size);

  if (result != ERROR_SUCCESS) {
    return std::unexpected("Couldn't get string data");
  }

  return text;
}
} // namespace registry
} // namespace windows

namespace filesystem {
auto create_directory(const std::filesystem::path& path) -> bool {
  return std::filesystem::create_directory(path) ||
         std::filesystem::exists(path);
}
} // namespace filesystem

namespace cs2 {
auto get_dir_path() noexcept
    -> std::expected<std::filesystem::path, std::string> {
  const auto key = HKEY_LOCAL_MACHINE;
  constexpr auto sub_key = L"SOFTWARE\\WOW6432Node\\Valve\\cs2";
  constexpr auto value = L"installpath";

  if (const auto dir = windows::registry::get_string(key, sub_key, value);
      dir.has_value()) {
    return dir.value().c_str();
  }

  return std::unexpected("Couldn't find CS2's dir in registry");
}

auto get_version(const std::filesystem::path& cs2_dir) noexcept
    -> std::expected<std::string, std::string> {
  const auto version_file_path = cs2_dir / "game" / "csgo" / "steam.inf";

  if (!std::filesystem::exists(version_file_path)) {
    return std::unexpected("Couldn't find CS2's version file");
  }

  auto file = std::ifstream(version_file_path);

  if (!file) {
    return std::unexpected("Couldn't open CS2's version file");
  }

  std::string line{};
  while (std::getline(file, line)) {
    if (line.starts_with("PatchVersion")) {
      return line.substr(13);
    }
  }

  return std::unexpected("Couldn't find PatchVersion in version file");
}
} // namespace cs2
} // namespace archiver

int main() {
  using namespace archiver;

  const auto cs2_dir = cs2::get_dir_path();
  if (!cs2_dir) {
    std::println("[!] {}", cs2_dir.error());
    return 0;
  }

  if (!std::filesystem::exists(cs2_dir.value())) {
    std::println("[!] CS2's directory does not exist");
    return 0;
  }

  std::println("[+] Dir: {}", cs2_dir.value().string());

  const auto patch_version = cs2::get_version(cs2_dir.value());
  if (!patch_version) {
    std::println("[!] {}", patch_version.error());
    return 0;
  }

  std::println("[+] Patch: {}", patch_version.value());

  const auto binaries_path = std::filesystem::current_path() / "binaries";
  if (!filesystem::create_directory(binaries_path)) {
    std::println("[!] Couldn't create {}", binaries_path.string());
    return 0;
  }

  const auto current_version_path = binaries_path / patch_version.value();
  if (std::filesystem::exists(current_version_path)) {
    std::println("[!] Backup of current patch already exists");
    return 0;
  } else if (!filesystem::create_directory(current_version_path)) {
    std::println("[!] Couldn't create {}", current_version_path.string());
    return 0;
  }

  const auto client_path = current_version_path / "client";
  if (!filesystem::create_directory(client_path)) {
    std::println("[!] Couldn't create {}", client_path.string());
    return 0;
  }

  const auto engine_path = current_version_path / "engine";
  if (!filesystem::create_directory(engine_path)) {
    std::println("[!] Couldn't create {}", engine_path.string());
    return 0;
  }

  const auto cs2_client_files_dir =
      cs2_dir.value() / "game" / "csgo" / "bin" / "win64";

  for (const auto file_name : { "client.dll", "server.dll" }) {
    const auto source = cs2_client_files_dir / file_name;
    const auto dest = client_path / file_name;
    if (!std::filesystem::copy_file(source, dest)) {
      std::println("[!] Couldn't copy {}", source.string());
    }
  }

  const auto cs2_engine_files_dir = cs2_dir.value() / "game" / "bin" / "win64";

  for (const auto file_name :
       { "cs2.exe", "engine2.dll", "materialsystem2.dll",
         "rendersystemdx11.dll", "schemasystem.dll", "tier0.dll",
         "networksystem.dll", "animationsystem.dll" }) {

    const auto source = cs2_engine_files_dir / file_name;
    const auto dest = engine_path / file_name;
    if (!std::filesystem::copy_file(source, dest)) {
      std::println("[!] Couldn't copy {}", source.string());
    }
  }

  std::println("[+] Finished :)");
}