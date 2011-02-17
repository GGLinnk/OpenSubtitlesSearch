/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "OpenSubtitlesRPCClient.h"
#include <QNetworkReply>
#include <QDomImplementation>
#include <QDomDocument>

QRegExp OpenSubtitlesRPCClient::REGEXP_OS_STATUS = QRegExp("(\\d+) (.+)$");

OpenSubtitlesRPCClient::OpenSubtitlesRPCClient(QObject* parent) : QObject(parent),
        m_manager(this), m_request(QUrl("http://api.opensubtitles.org/xml-rpc"))
{
	QDomImplementation::setInvalidDataPolicy(QDomImplementation::DropInvalidChars);
	
	setUserAgent("mylibmaia/0.1");
	m_request.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml");
	
        QObject::connect(&m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

QNetworkReply* OpenSubtitlesRPCClient::call(const QString & method, const QList<QVariant> & args, REQUEST_TYPE rtype) {
	QDomDocument doc;

	QDomProcessingInstruction header = doc.createProcessingInstruction( "xml", QString("version=\"1.0\" encoding=\"UTF-8\"" ));
	doc.appendChild(header);
	
	QDomElement methodCall = doc.createElement("methodCall");
	QDomElement methodName = doc.createElement("methodName");
	QDomElement params = doc.createElement("params");
	QDomElement param;

	doc.appendChild(methodCall);
	methodCall.appendChild(methodName);
	methodName.appendChild(doc.createTextNode(method));

	methodCall.appendChild(params);

	for(int i = 0; i < args.size(); ++i) {
		param = doc.createElement("param");
		param.appendChild(toXml(args.at(i)));
		params.appendChild(param);
	}
	
	QNetworkReply* reply = m_manager.post( m_request, doc.toByteArray() );
	m_callmap[reply] = rtype;
	return reply;
}

void OpenSubtitlesRPCClient::replyFinished(QNetworkReply* reply) {
        if(!m_callmap.contains(reply)) return;
	
	if(reply->error() != QNetworkReply::NoError) {
                emit error(reply->error(), reply->errorString(), reply);
	} else {
		QString response = QString::fromUtf8(reply->readAll());

		QDomDocument doc;
		QVariant arg;
		QString errorMsg;
		int errorLine;
		int errorColumn;
		if(!doc.setContent(response, &errorMsg, &errorLine, &errorColumn)) {
                        emit error(1000, trUtf8("Error while parsing xml response (line %1, column %2):\n%3").arg(errorLine).arg(errorColumn).arg(errorMsg), reply);
		}
		else {
			QString tagName = doc.documentElement().firstChild().toElement().tagName().toLower();
			if(tagName == "params") {
				QDomNode paramNode = doc.documentElement().firstChild().firstChild();
				if(!paramNode.isNull()) {
					arg = fromXml( paramNode.firstChild().toElement() );
				}
                                QMap<QString, QVariant> ans = arg.toMap();
                                if (! REGEXP_OS_STATUS.exactMatch(ans.value("status").toString())) {
                                        emit error(1100, trUtf8("Non conform response from OpenSubtitles: No status field."), reply);
				} else {
					int err = REGEXP_OS_STATUS.cap(1).toInt();
					if (err != 200) {
                                                emit error(- err, trUtf8("Response of OpenSubitles server :\n%1").arg(REGEXP_OS_STATUS.cap(2)), reply);
					}
					else {
                                            emitResponse(reply, m_callmap[reply], ans);
					}
				}
			} else if(tagName == "fault") {
				QVariant errorVariant = fromXml(doc.documentElement().firstChild().firstChild().toElement());
                                emit error(errorVariant.toMap() [ "faultCode" ].toInt(), errorVariant.toMap() [ "faultString" ].toString(), reply);
			} else {
                                emit error(1200, trUtf8("Non compliant to XML-RPC."), reply);
			}
		}
	}
	reply->deleteLater();
        m_callmap.remove(reply);
}

OpenSubtitlesRPCClient::REQUEST_TYPE OpenSubtitlesRPCClient::replyType(QNetworkReply *reply) {
    QHash<QNetworkReply *, REQUEST_TYPE>::const_iterator i = m_callmap.find(reply);
    return (i == m_callmap.end()) ? ERROR : i.value();
}
