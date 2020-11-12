install: install-matterialize install-matterialize-server

install-matterialize:
	cd matterialize-react-gui && make install-linux-generic

install-matterialize-server:
	./install/install.sh

uninstall: uninstall-matterialize uninstall-matterialize-server

uninstall-matterialize:
	cd matterialize-react-gui && make uninstall-linux-generic

uninstall-matterialize-server:
	sudo rm -rf /usr/local/bin/matterialize-server

.PHONY: install uninstall
