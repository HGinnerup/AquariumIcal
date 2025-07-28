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

    # Extract TELNET_PORT (integer)
    telnet_port_match = re.search(r'#define\s+TELNET_PORT\s+(\d+)', content)
    if telnet_port_match:
        values['TELNET_PORT'] = int(telnet_port_match.group(1))

    return values



def populate_telnet_info(env):
    cppConfig = extract_config_values(CONFIG_PATH)

    env.Append(MONITOR_PORT = f'socket://{cppConfig["ESP_NETWORK_HOSTNAME"]}:{cppConfig["TELNET_PORT"]}')


print("Populating telnet")
Import("env")
env.AddPreAction("monitor", lambda: populate_telnet_info(env))
# print(env.Dump())