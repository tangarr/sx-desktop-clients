/*
 *  Copyright (C) 2012-2016 Skylable Ltd. <info-copyright@skylable.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  Special exception for linking this software with OpenSSL:
 *
 *  In addition, as a special exception, Skylable Ltd. gives permission to
 *  link the code of this program with the OpenSSL library and distribute
 *  linked combinations including the two. You must obey the GNU General
 *  Public License in all respects for all of the code used other than
 *  OpenSSL. You may extend this exception to your version of the program,
 *  but you are not obligated to do so. If you do not wish to do so, delete
 *  this exception statement from your version.
 */

#include <QDragEnterEvent>
#include <QMimeData>
#include <QUrl>
#include <QLabel>
#include <QMovie>
#include <QHBoxLayout>

#include "dropframe.h"
#include "sxlog.h"

#include <QDebug>

DropFrame::DropFrame(QWidget * parent, Qt::WindowFlags f) : QFrame(parent, f) {
    setAcceptDrops(true);
    QMovie *movie = new QMovie(":ani/dropframe");
    QLabel *labelDrop = new QLabel;
    labelDrop->setMovie(movie);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(labelDrop);
    setLayout(layout);
    movie->start();
    
}

void DropFrame::dragEnterEvent(QDragEnterEvent *event) {
    if(!event->mimeData()->hasUrls())
	return;
    QList<QUrl> urls = event->mimeData()->urls();
    for(int i=0; i<urls.size(); i++) {
	QUrl url = urls.at(i);
	if(!url.isLocalFile())
	    continue;
	event->acceptProposedAction();
	return;
    }
}

void DropFrame::dropEvent(QDropEvent *event) {
    if(!event->mimeData()->hasUrls())
	return;

    QList<QUrl> urls = event->mimeData()->urls();
    SxUrl sxurl;

    for(int i=0; i<urls.size(); i++) {
	QUrl url = urls.at(i);
	if(!url.isLocalFile())
	    continue;
	QFile f(urls.at(i).toLocalFile());
	if(!f.exists())
	    continue;
	if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
	    continue;
	QTextStream fs(&f);
	QString line;
	while (line.trimmed().isEmpty()) {
	    line = fs.readLine();
	    if (line.isNull())
		break;
	}
	sxurl = SxUrl(line);
	if(!sxurl.isValid()) {
	    logWarning(QString("Url %1 from file %2 is not valid").arg(line).arg(f.fileName()));
	    continue;
	}
	logVerbose("Accepting drop event for "+ sxurl.url());
	event->acceptProposedAction();
	break;
    }

    emit(urlDropped(sxurl));
}