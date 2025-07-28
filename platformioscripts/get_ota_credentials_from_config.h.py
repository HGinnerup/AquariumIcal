import re


CONFIG_PATH = "src/common/config.h"



def extract_config_values(config_path):
    values = {}
    with open(config_path, 'r') as file:
        content = file.read()

    # Extract ESP_NETWORK_HOSTNAME (string in quotes)
    hostname_match = re.search(r'#define\s+ESP_NETWORK_HOSTNAME\s+"(.+?)"', content)
    if hostname_match:
        values['ESP_NETWORK_HOSTNAME'] = hostname_match.group(1)

    # Extract OTA_PORT (integer)
    ota_port_match = re.search(r'#define\s+OTA_PORT\s+(\d+)', content)
    if ota_port_match:
        values['OTA_PORT'] = int(ota_port_match.group(1))

    # Extract OTA_PASSWORD (string in quotes)
    ota_password_match = re.search(r'#define\s+OTA_PASSWORD\s+"(.+?)"', content)
    if ota_password_match:
        values['OTA_PASSWORD'] = ota_password_match.group(1)

    return values


def populate_ota_info(env):
    cppConfig = extract_config_values(CONFIG_PATH)

    env.Append(UPLOAD_PORT = cppConfig["ESP_NETWORK_HOSTNAME"])

    if cppConfig["OTA_PORT"] != "":
        env.Append(UPLOAD_FLAGS = [f"--port={cppConfig['OTA_PORT']}"])

    if cppConfig["OTA_PASSWORD"] != "":
        env.Append(UPLOAD_FLAGS = ["--auth=" + cppConfig["OTA_PASSWORD"]])


Import("env")
populate_ota_info(env)
print(env.Dump())