/*
 * ODataConstants.c
 *
 *  Created on: 2013-04-01
 *      Author: Daniel Baxter
 */


const char* METADATA = "$metadata";
const char* FORMAT = "$format";

const char* COLLECTION = "collection";
const char* ENTITYSETS = "EntitySets";
const char* WORKSPACE = "workspace";

const char* ASSOCIATION = "Association";
const char* COMPLEX_TYPE = "ComplexType";
const char* DATA = "Data";
const char* EDMX = "edmx:Edmx";
const char* EDMX_DATASERVICES = "edmx:DataServices";
const char* END_COMPLEX_TYPE = "EndComplexType";
const char* ENTITY_TYPE = "EntityType";
const char* FROM_ROLE = "FromRole";
const char* NAME = "Name";
const char* NAVIGATION_PROPERTY = "NavigationProperty";
const char* NULLABLE = "Nullable";
const char* PROPERTY = "Property";
const char* RELATIONSHIP = "Relationship";
const char* SCHEMA = "Schema";
const char* TYPE = "Type";

const char* EDM = "Edm.";
const char* EDM_BINARY = "Edm.Binary";
const char* EDM_BOOLEAN = "Edm.Boolean";
const char* EDM_BYTE = "Edm.Byte";
const char* EDM_DATETIME = "Edm.DateTime";
const char* EDM_DATETIMEOFFSET = "Edm.DateTimeOffset";
const char* EDM_DECIMAL = "Edm.Decimal";
const char* EDM_DOUBLE = "Edm.Double";
const char* EDM_GUID = "Edm.Guid";
const char* EDM_INT16 = "Edm.Int16";
const char* EDM_INT32 = "Edm.Int32";
const char* EDM_INT64 = "Edm.Int64";
const char* EDM_SBYTE = "Edm.SByte";
const char* EDM_SINGLE = "Edm.Single";
const char* EDM_STRING = "Edm.String";
const char* EDM_TIME = "Edm.Time";


const char* XML_TAG = "<?xml version='1.0' encoding='utf-8'?>";
const char* ENTRY_OPEN = "<entry xmlns:d='http://schemas.microsoft.com/ado/2007/08/dataservices' "
        "xmlns:m='http://schemas.microsoft.com/ado/2007/08/dataservices/metadata' "
        "xmlns='http://www.w3.org/2005/Atom'>"
            "<title type='text'></title>"
            "<author>"
                "<name />"
            "</author>";
const char* ENTRY_CLOSE = "</entry>";
const char* UPDATED = "updated";
const char* CATEGORY_OPEN = "<category term='";
const char* CATEGORY_CLOSE = "' scheme='http://schemas.microsoft.com/ado/2007/08/dataservices/scheme' /> ";
const char* CONTENT_OPEN = "<content type='application/xml'><m:properties>";
const char* CONTENT_CLOSE = "</m:properties></content>";
const char* LINK_TEMPLATE = "<link rel='{{schema}}' type='application/atom+xml;type=entry' title='{{title}}' href='{{link}}' />";
