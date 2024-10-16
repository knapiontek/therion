hello:
	address:
		town: unicode
		street: unicode
		house-no: int
		flat-no: 22

	address(this-town: unicode, flat-no: int): hello.address
		town: this-town
		flat-no: flat-no

	address(birthday: unicode): hello.address("Poznan", 163)
		birthday: birthday

	my-address: hello.address("25-03-75")
		my-name: "Kris"

	your-address: hello.address
		help: 0

	profile:
		first-name: "Kris"
		last-name: unicode
		address: hello.address("Poznan", 163)

apply-button: button
	title: "Apply"

custom-button(post: hello.address): button
	title: post.town

my-label(title: unicode): label
	title: title

my-edit(title: unicode): edit
	title: title

profile-dialog: dialog
	title: "Profile"
	pane1: pane
		pane1: pane
			my-label("First Name")
			my-edit(hello.profile.first-name)
		pane2: pane
			my-label("Town")
			my-edit("---")
		pane3: pane
			apply-button
			custom-button(hello.your-address)

main-layout: layout
	design: headline
	pane1: pane
		region: top
		class: edge-panel
		content: "Header content (top)"
	pane2: pane
		region: left
		id: left-col
		class: edge-panel
		content: "Sidebar content (left)"
		splitter: true
	pane3: tab-pane
		region: center
		id: content-tabs
		tab-position: bottom
		class: center-panel
