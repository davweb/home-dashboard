
"""Merge configuration from environment variables and the command line"""

import argparse
import logging
import os


def _get_arguments() -> argparse.Namespace:
    """Parse command line arguments"""

    parser = argparse.ArgumentParser(prog='dashboard', description='Server for Home Dashboard.')
    parser.add_argument('-p', '--port', action='store', type=int, help='Port to listen on')
    parser.add_argument('-b', '--bus-stop-id', action='append', help='Bus Stop ID')
    parser.add_argument('-l', '--lat-long', action='store', help='Latitude and Longitude for weather')
    parser.add_argument('-k', '--pirate-api-key', action='store', help='API Key for Pirate Weather')
    parser.add_argument('-r', '--recycling-calendar-url', action='store', help='URL for recycling collection calendar')
    parser.add_argument('-n', '--unifi-host', action='store', help='Unifi Controller hostname')
    parser.add_argument('-u', '--unifi-username', action='store', help='Unifi Controller username')
    parser.add_argument('-w', '--unifi-password', action='store', help='Unifi Controller password')
    parser.add_argument('-m', '--unifi-mac', action='append', help='Unifi Controller password')

    return parser.parse_args()


class Config:
    """Configuration class"""

    def __init__(self) -> None:
        self._args = _get_arguments()

    @property
    def port(self) -> int:
        """Return the port to listen on"""

        if self._args.port:
            return self._args.port

        if 'DASHBOARD_PORT' in os.environ:
            return int(os.environ['DASHBOARD_PORT'])

        return 8000

    @property
    def bus_stop_ids(self) -> list[str]:
        """Bus Stops to fetch data for"""

        if self._args.bus_stop_id:
            return self._args.bus_stop_id

        if 'BUS_STOP_IDS' in os.environ:
            return os.environ['BUS_STOP_IDS'].split(',')

        raise ValueError('No bus stop IDs provided')

    @property
    def lat_long(self) -> str:
        """Lat-long for weather"""

        if self._args.lat_long:
            return self._args.lat_long

        if 'LAT_LONG' in os.environ:
            return os.environ['LAT_LONG']

        raise ValueError('No lat-long provided')

    @property
    def recycling_calendar_url(self) -> str:
        """URL for Recycling Collection Calendar"""

        if self._args.lat_long:
            return self._args.recycling_calendar_url

        if 'RECYCLING_CALENDAR_URL' in os.environ:
            return os.environ['RECYCLING_CALENDAR_URL']

        raise ValueError('No recycling calendar provided')

    @property
    def pirate_api_key(self) -> str:
        """API Key for Pirate Weather"""

        if self._args.pirate_api_key:
            return self._args.pirate_api_key

        if 'PIRATE_API_KEY' in os.environ:
            return os.environ['PIRATE_API_KEY']

        raise ValueError('No Pirate API Key provided')

    @property
    def log_level(self) -> int:
        """Log Level"""

        log_level_name = os.environ.get('LOG_LEVEL', 'INFO')
        log_level = logging.getLevelName(log_level_name)

        if isinstance(log_level, str):
            raise ValueError(f'Invalid LOG_LEVEL: {log_level_name}')

        return log_level

    @property
    def unifi_hostname(self) -> str:
        """Unifi Controller hostname"""

        if self._args.unifi_host:
            return self._args.unifi_host

        if 'UNIFI_HOST' in os.environ:
            return os.environ['UNIFI_HOST']

        raise ValueError('No Unifi Controller hostname provided')

    @property
    def unifi_username(self) -> str:
        """Unifi Controller username"""

        if self._args.unifi_username:
            return self._args.unifi_username

        if 'UNIFI_USERNAME' in os.environ:
            return os.environ['UNIFI_USERNAME']

        raise ValueError('No Unifi Controller username provided')

    @property
    def unifi_password(self) -> str:
        """Unifi Controller password"""

        if self._args.unifi_password:
            return self._args.unifi_password

        if 'UNIFI_PASSWORD' in os.environ:
            return os.environ['UNIFI_PASSWORD']

        raise ValueError('No Unifi Controller password provided')

    @property
    def unifi_macs(self) -> dict[str, str]:
        """Unifi Controller MACs"""

        if self._args.unifi_mac:
            macs = {}

            for pair in self._args.unifi_mac:
                name, mac = pair.split('=')
                macs[name] = mac

            return macs

        if 'UNIFI_MACS' in os.environ:
            macs = {}

            for pair in os.environ['UNIFI_MACS'].split(','):
                name, mac = pair.split('=')
                macs[name] = mac

            return macs

        raise ValueError('No Unifi Controller macs to monitor provided')


CONFIG = Config()
