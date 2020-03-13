#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import os
import sys
import traceback
import logging
import yaml



# parse sdk name & version
def parse_sdk_yaml(sdk_yaml):
    repo_url = ''
    sdk_name = ''
    sdk_version = ''
    with open(sdk_yaml, "r") as r:
        sdk_yaml_content = yaml.load(r.read(),Loader=yaml.Loader)
        for dependency in sdk_yaml_content.get('dependencies', []):
            #print("dependency", dependency)
            for module_name in dependency:
                sdk_name = module_name
                sdk_version = dependency[module_name]

    return repo_url, sdk_name, sdk_version


# parse sdk package url
def parse_sdk_module_yaml(sdk_module_yaml):
    sdk_package_url = ''
    with open(sdk_module_yaml, 'r') as r:
        sdk_module_content = yaml.load(r.read(), Loader=yaml.Loader)
        #print("sdk_module_content:", sdk_module_content)
        spec = sdk_module_content.get('spec', '')
        source = spec.get('source', '')
        sdk_package_url = source.get('https', '')

    return sdk_package_url


def main():
    logging.basicConfig(level=logging.INFO)

    sdk_yaml = sys.argv[1]
    repo_path = sys.argv[2]

    try:
        _, sdk_name, sdk_version = parse_sdk_yaml(sdk_yaml)
        sdk_module_yaml = os.path.join(repo_path, sdk_name, sdk_version, sdk_name + '.yaml')
        #print('sdk module yaml:', sdk_module_yaml)
        sdk_package_url = parse_sdk_module_yaml(sdk_module_yaml)
        print(sdk_package_url)
        exit(0)

    except Exception as e:
        traceback.print_exc()
        logging.error(str(e))
        exit(1)


if __name__ == '__main__':
    main()

