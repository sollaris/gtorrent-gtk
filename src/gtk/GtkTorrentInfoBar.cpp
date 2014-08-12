#include "GtkTorrentInfoBar.hpp"

#include <gtkmm/table.h>
#include <gtkmm/hvseparator.h>

/**
* Sets up the torrent info bar.
*/
GtkTorrentInfoBar::GtkTorrentInfoBar()
	: Gtk::Box(Gtk::ORIENTATION_VERTICAL)
{
	//TODO: better layout
	m_notebook = Gtk::manage(new Gtk::Notebook());
	m_stat_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_piece_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));

	m_title = Gtk::manage(new Gtk::Label());

	this->pack_start(*m_title, Gtk::PACK_SHRINK);

	m_progress = Gtk::manage(new GtkBlockBar());
	m_graph = Gtk::manage(new GtkGraph());

	m_piece_box->pack_end(*m_progress, Gtk::PACK_EXPAND_WIDGET, 0);
	m_progress->set_size_request(0, 50);


	m_stat_box->add(*m_piece_box);

	m_table_layout = Gtk::manage(new Gtk::Table(3, 2, false));
	m_table_layout->set_col_spacings(5);

	m_down_total_label = Gtk::manage(new Gtk::Label());
	m_down_total_label->set_markup("<b>Downloaded</b>");
	m_down_total = Gtk::manage(new Gtk::Label("0"));
	m_table_layout->attach(*m_down_total_label, 0, 1, 0, 1, Gtk::AttachOptions::SHRINK);
	m_table_layout->attach(*(new Gtk::VSeparator()), 1, 2, 0, 1, Gtk::AttachOptions::SHRINK);
	m_table_layout->attach(*m_down_total, 2, 3, 0, 1, Gtk::AttachOptions::SHRINK);

	m_up_total_label = Gtk::manage(new Gtk::Label());
	m_up_total_label->set_markup("<b>Uploaded</b>");
	m_up_total = Gtk::manage(new Gtk::Label("0"));
	m_table_layout->attach(*m_up_total_label, 0, 1, 1, 2, Gtk::AttachOptions::SHRINK);
	m_table_layout->attach(*(new Gtk::VSeparator()), 1, 2, 1, 2, Gtk::AttachOptions::SHRINK);
	m_table_layout->attach(*m_up_total, 2, 3, 1, 2, Gtk::AttachOptions::SHRINK);

	m_stat_box->pack_start(*(new Gtk::HSeparator()), Gtk::PACK_SHRINK);
	m_stat_box->pack_start(*m_table_layout, Gtk::PACK_SHRINK);
	m_notebook->append_page(*m_graph, "Info Graph");
	m_notebook->append_page(*m_stat_box, "Torrent Info");
	this->pack_end(*m_notebook, Gtk::PACK_EXPAND_WIDGET, 5);
}

// TODO: Should replace every place where a torrent index is required with a torrent pointer, smells like everything would break if
// the user tried to sort the torrents.

/**
* Updates the torrent info bar.
*/
void GtkTorrentInfoBar::updateInfo(shared_ptr<gt::Torrent> selected)
{
	static shared_ptr<gt::Torrent> previous = nullptr;
	int selectedIndex = 0;
	vector<shared_ptr<gt::Torrent>> t = Application::getSingleton()->getCore()->getTorrents();

	if(selected)
		set_visible(true);
	else
	{
		set_visible(false);
		return;
	}

	for(unsigned i = 0; i < t.size(); ++i)
		if(selected == t[i])
			selectedIndex = i;

	if(t[selectedIndex]->getHandle().status().has_metadata) // torrentless torrents (magnet links) can't have pieces
		m_progress->setBlocks(t[selectedIndex]->getPieces());

	m_title->set_text(t[selectedIndex]->getName());
	m_graph->select(selectedIndex);

	if(previous != selected)
	{
		m_down_total->set_text(t[selectedIndex]->getTextTotalDownloaded());
		m_up_total->set_text(t[selectedIndex]->getTextTotalUploaded());
	}
	previous = selected;
}

void GtkTorrentInfoBar::updateState(shared_ptr<gt::Torrent> selected)
{
	if(!selected) return updateInfo(selected);
	int selectedIndex = 0;
	vector<shared_ptr<gt::Torrent>> t = Application::getSingleton()->getCore()->getTorrents();
	for(unsigned i = 0; i < t.size(); ++i)
		if(selected == t[i])
			selectedIndex = i;
	if(t[selectedIndex]->getHandle().status().has_metadata)
		m_progress->setBlocks(t[selectedIndex]->getPieces());
	m_down_total->set_text(t[selectedIndex]->getTextTotalDownloaded());
	m_up_total->set_text(t[selectedIndex]->getTextTotalUploaded());

	for(unsigned i = 0; i < t.size(); ++i)
		m_graph->add(i, (double)t[i]->getUploadRate(), (double)t[i]->getDownloadRate());

}
