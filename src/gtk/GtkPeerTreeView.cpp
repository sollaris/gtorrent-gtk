#include "GtkPeerTreeView.hpp"

#include <gtkmm/liststore.h>

void GtkPeerTreeView::insert(const gt::Peer &p)
{
	Gtk::TreeModel::iterator i = m_liststore->children().begin();
	while(i != m_liststore->children().end() && std::string((*i)[m_cols.m_col_ip]) < p.ip)
		++i;
	if(i != m_liststore->children().end() && std::string((*i)[m_cols.m_col_ip]) == p.ip)
	{
		(*i)[m_cols.m_col_ip] = p.ip;
		(*i)[m_cols.m_col_client] = p.client;
		(*i)[m_cols.m_col_down] = p.downloadSpeed;
		(*i)[m_cols.m_col_up] = p.uploadSpeed;
	}
	else
	{
		i = m_liststore->insert(i);
		(*i)[m_cols.m_col_ip] = p.ip;
		(*i)[m_cols.m_col_client] = p.client;
		(*i)[m_cols.m_col_down] = p.downloadSpeed;
		(*i)[m_cols.m_col_up] = p.uploadSpeed;
	}
}

GtkPeerTreeView::GtkPeerTreeView()
{
	m_liststore = Gtk::ListStore::create(m_cols);
	set_model(m_liststore);
	setupColumns();
}


void GtkPeerTreeView::update()
{
	for (auto &peer : torrent->getPeers())
		insert(peer);
}

void GtkPeerTreeView::setupColumns()
{
	append_column("IP", m_cols.m_col_ip);
	append_column("Client", m_cols.m_col_client);
	append_column("Download", m_cols.m_col_down);
	append_column("Upload", m_cols.m_col_up);
}

void GtkPeerTreeView::select(std::shared_ptr<gt::Torrent> t)
{
	torrent = t;
	m_liststore->clear();
	update();
}