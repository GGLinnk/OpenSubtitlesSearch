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
#include <QDateTime>
#include <QDebug>

QDomElement OpenSubtitlesRPCClient::toXml(const QVariant & arg) {
	
	//dummy document
	QDomDocument doc;
	//value element, we need this in each case
	QDomElement tagValue = doc.createElement("value");

	switch(arg.type()) {
	case QVariant::String: {

		QDomElement tagString = doc.createElement("string"); 
		QDomText textString = doc.createTextNode(arg.toString());
		
		tagValue.appendChild(tagString);
		tagString.appendChild(textString);

		return tagValue;

	} case QVariant::Int: {

		QDomElement tagInt = doc.createElement("int"); 
		QDomText textInt = doc.createTextNode(QString::number(arg.toInt()));
		
		tagValue.appendChild(tagInt);
		tagInt.appendChild(textInt);

		return tagValue;

	} case QVariant::Double: {

		QDomElement tagDouble = doc.createElement("double"); 
		QDomText textDouble = doc.createTextNode(QString::number(arg.toDouble()));
		
		tagValue.appendChild(tagDouble);
		tagDouble.appendChild(textDouble);

		return tagValue;

	} case QVariant::Bool: {
	
		QString textValue = arg.toBool() ? "1" : "0";

		QDomElement tag = doc.createElement("boolean"); 
		QDomText text = doc.createTextNode(textValue);
		
		tagValue.appendChild(tag);
		tag.appendChild(text);

		return tagValue;

	} case QVariant::ByteArray: {

		QString textValue = arg.toByteArray().toBase64();

		QDomElement tag = doc.createElement("base64"); 
		QDomText text = doc.createTextNode(textValue);
		
		tagValue.appendChild(tag);
		tag.appendChild(text);

		return tagValue;

	} case QVariant::DateTime: {
	
		QString textValue = arg.toDateTime().toString("yyyyMMddThh:mm:ss");

		QDomElement tag = doc.createElement("datetime.iso8601"); 
		QDomText text = doc.createTextNode(textValue);
		
		tagValue.appendChild(tag);
		tag.appendChild(text);

		return tagValue;

	} case QVariant::List: {

		QDomElement tagArray = doc.createElement("array");
		QDomElement tagData = doc.createElement("data");
		tagArray.appendChild(tagData);
		tagValue.appendChild(tagArray);

		const QList<QVariant> args = arg.toList();
		for(int i = 0; i < args.size(); ++i) {
			tagData.appendChild(toXml(args.at(i)));
		}
	
		return tagValue;

	} case QVariant::Map: {

		QDomElement tagStruct = doc.createElement("struct");
		QDomElement member;
		QDomElement name;

		tagValue.appendChild(tagStruct);

		QMap<QString, QVariant> map = arg.toMap();
		QMapIterator<QString, QVariant> i(map);
		while(i.hasNext()) {
			i.next();

			member = doc.createElement("member");
			name = doc.createElement("name");

			// (key) -> name -> member -> struct
			tagStruct.appendChild(member);
			member.appendChild(name);
			name.appendChild(doc.createTextNode(i.key()));

			// add variables by recursion
			member.appendChild(toXml(i.value()));
		}

		return tagValue;

	} default:
                qDebug() << "Failed to marshal unknown variant type: " << arg.type();
	}
	return QDomElement(); //QString::null;
}

QVariant OpenSubtitlesRPCClient::fromXml(const QDomElement &elem) {
	if(elem.tagName().toLower() != "value") {
		return QVariant();
	}
	
	// If no type is indicated, the type is string.
	if(!elem.firstChild().isElement()) {
		return QVariant(elem.text());
	}
	
	const QDomElement typeElement = elem.firstChild().toElement();	
	const QString typeName = typeElement.tagName().toLower();

	if(typeName == "string")
		return QVariant(typeElement.text());
	else if(typeName == "i4" || typeName == "int")
		return QVariant(typeElement.text().toInt());
	else if(typeName == "double")
		return QVariant(typeElement.text().toDouble());
	else if (typeName == "boolean") {
		if(typeElement.text().toLower() == "true" || typeElement.text() == "1")
			return QVariant(true);
		else
			return QVariant(false);
	} else if(typeName == "base64")
		return QVariant(QByteArray::fromBase64( typeElement.text().toLatin1()));
	else if(typeName == "datetime" || typeName == "datetime.iso8601")
		return QVariant(QDateTime::fromString(typeElement.text(), "yyyyMMddThh:mm:ss"));
	else if(typeName == "nil") // Non-standard extension: http://ontosys.com/xml-rpc/extensions.php
		return QVariant();
	else if ( typeName == "array" ) {
		QList<QVariant> values;
		QDomNode valueNode = typeElement.firstChild().firstChild();
		while(!valueNode.isNull()) {
			values << fromXml(valueNode.toElement());
			valueNode = valueNode.nextSibling();
		}
		return QVariant(values);
	}
	else if ( typeName == "struct" ) {
		QMap<QString, QVariant> map;
		QDomNode memberNode = typeElement.firstChild();
		while(!memberNode.isNull())	{
			const QString key = memberNode.toElement().elementsByTagName("name").item(0).toElement().text();
			const QVariant data = fromXml(memberNode.toElement().elementsByTagName("value").item(0).toElement());
			map[key] = data;
			memberNode = memberNode.nextSibling();
		}
		return QVariant(map);
	} else {
		qDebug() << "Cannot demarshal unknown type " << typeElement.tagName().toLower();
	}
	return QVariant();
}

