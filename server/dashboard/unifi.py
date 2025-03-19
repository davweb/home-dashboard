"""Query status from the local Unifi Controller"""

import time
from pyunifi.controller import Controller
from .config import CONFIG

CONTROLLER = Controller(CONFIG.unifi_hostname, CONFIG.unifi_username, CONFIG.unifi_password, ssl_verify=False)


def get_online_status() -> dict[str, bool]:
    """Return the online status of the configured devices"""
    now = int(time.time())
    online = {client['mac']: client['last_seen'] for client in CONTROLLER.get_clients()}
    return {name: mac in online and online[mac] - now < 60 for name, mac in CONFIG.unifi_macs.items()}
