#!/bin/bash
mkdir ./projects_folder
touch ./bin/settings.ini
echo "ngsd_test_host = \"127.0.0.1\"" >> ./bin/settings.ini
echo "ngsd_test_port = 3306" >> ./bin/settings.ini
echo "ngsd_test_name = \"root\"" >> ./bin/settings.ini
echo "ngsd_test_user = \"root\"" >> ./bin/settings.ini
echo "ngsd_test_pass = \"\"" >> ./bin/settings.ini
echo "projects_folder = \"projects_folder\"" >> ./bin/settings.ini
echo "data_folder = \"\"" >> ./bin/settings.ini
