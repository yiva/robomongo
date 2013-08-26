#include "robomongo/core/domain/MongoDocument.h"

#include <mongo/client/dbclient.h>
#include <QStringBuilder>

#include "robomongo/core/domain/MongoDocumentIterator.h"
#include "robomongo/core/domain/MongoElement.h"

namespace Robomongo
{
    Concatenator::Concatenator():_count(0)
    {
    }

    void Concatenator::append(const QString &data)
    {
        _list.append(data);
        _count += data.length();
    }

    QString Concatenator::build()
    {
        QString text;
        text.reserve(_count + 10);
        for (QStringList::const_iterator it=_list.begin();it!=_list.end();++it){
            text = text % (*it);
        }

        return text;
    }

    MongoDocument::MongoDocument()
    {
    // test
    }

    MongoDocument::~MongoDocument()
    {
    }

    /*
    ** Create MongoDocument from BsonObj. It will take owned version of BSONObj
    */
    MongoDocument::MongoDocument(mongo::BSONObj bsonObj):_bsonObj(bsonObj)
    {
    }

    /*
    ** Create MongoDocument from BsonObj. It will take owned version of BSONObj
    */ 
    MongoDocumentPtr MongoDocument::fromBsonObj(const mongo::BSONObj &bsonObj)
    {
        MongoDocument * doc = new MongoDocument(bsonObj);
        return MongoDocumentPtr(doc);
    }

    /*
    ** Create list of MongoDocuments from QList<BsonObj>. It will take owned version of BSONObj
    */ 
    std::vector<MongoDocumentPtr> MongoDocument::fromBsonObj(const std::vector<mongo::BSONObj> &bsonObjs)
    {
        std::vector<MongoDocumentPtr> list;
        for(std::vector<mongo::BSONObj>::const_iterator it=bsonObjs.begin();it!=bsonObjs.end();++it){
            list.push_back(fromBsonObj(*it));
        }

        return list;
    }

    /*
    ** Convert to json string
    */
    void MongoDocument::buildJsonString(Concatenator &con)
    {
        MongoDocumentIterator i(this);
        con.append("{ \n");
        while (i.hasMore())
        {
            MongoElementPtr e = i.next();

            con.append("\"");
            con.append(e->fieldName());
            con.append("\"");
            con.append(" : ");
            e->buildJsonString(con);
            con.append(", \n");
        }

        con.append("\n}\n\n");
    }

    /*
    ** Build JsonString from list of documents
    */
    std::string MongoDocument::buildJsonString(const QList<MongoDocumentPtr> &documents)
    {
        mongo::StringBuilder sb;

        int position = 0;
        for(QList<MongoDocumentPtr>::const_iterator it=documents.begin();it!=documents.end();++it)
        {
            if (position == 0)
                sb << "/* 0 */\n";
            else 
                sb << "\n\n/* " << position << " */\n";

            std::string jsonString = (*it)->bsonObj().jsonString(mongo::TenGen, 1);

            sb << jsonString;
            position++;
        }

        std::string final = sb.str();
        return final;
    }

    std::string MongoDocument::buildJsonString(const MongoDocumentPtr &doc)
    {
        //qt4 QTextCodec::setCodecForCStrings(codec);
        std::string jsonString = doc->bsonObj().jsonString(mongo::TenGen, 1);
        return jsonString;
    }

}
