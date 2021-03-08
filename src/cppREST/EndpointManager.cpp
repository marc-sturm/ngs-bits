#include "EndpointManager.h"

EndpointManager::EndpointManager()
{
}

EndpointManager::~EndpointManager()
{
}

ParamProps::ParamType EndpointManager::getParamTypeFromString(QString in)
{
	if (in.toLower() == "string") return ParamProps::ParamType::STRING;
	if ((in.toLower() == "int") || (in.toLower() == "integer")) return ParamProps::ParamType::INTEGER;

	return ParamProps::ParamType::UNKNOWN;
}

void EndpointManager::validateInputData(Endpoint* current_endpoint, Request request)
{	
	QMapIterator<QString, ParamProps> i(current_endpoint->params);
	while (i.hasNext()) {
		i.next();		
		bool is_found = false;

		if(i.value().category == ParamProps::ParamCategory::POST_URL_ENCODED)
		{
			if (request.form_urlencoded.contains(i.key()))
			{
				is_found = true;
				if (!isParamTypeValid(request.form_urlencoded[i.key()], current_endpoint->params[i.key()].type))
				{
					THROW(ArgumentException, i.key() + " x-www-form-urlencoded parameter has an invalid type");
				}
			}
		}

		if(i.value().category == ParamProps::ParamCategory::GET_URL_PARAM)
		{
			if (request.url_params.contains(i.key()))
			{
				is_found = true;				
				if (!isParamTypeValid(request.url_params[i.key()], current_endpoint->params[i.key()].type))
				{
					THROW(ArgumentException, i.key() + " parameter inside URL has an invalid type");
				}
			}
		}

		if(i.value().category == ParamProps::ParamCategory::PATH_PARAM)
		{
			if (request.path_params.size()>0)
			{
				is_found = true;
			}
		}

		if ((!i.value().is_optional) && (!is_found))
		{
			THROW(ArgumentException, "Parameter " + i.key() + " is missing");
		}
	}
}

void EndpointManager::appendEndpoint(Endpoint new_endpoint)
{
	if (!instance().endpoint_registry_.contains(new_endpoint))
	{
		instance().endpoint_registry_.append(new_endpoint);
	}
}

QString EndpointManager::generateGlobalHelp()
{
	return getEndpointHelpTemplate(&instance().endpoint_registry_);
}

QString EndpointManager::generateEntityHelp(QString path, Request::MethodType method)
{
	QList<Endpoint> selected_endpoints {};
	selected_endpoints.append(getEndpointEntity(path, method));
	return getEndpointHelpTemplate(&selected_endpoints);
}

EndpointManager& EndpointManager::instance()
{
	static EndpointManager endpoint_factory;
	return endpoint_factory;
}

bool EndpointManager::isParamTypeValid(QString param, ParamProps::ParamType type)
{
	switch (type)
	{
		case ParamProps::ParamType::STRING: return true;
		case ParamProps::ParamType::INTEGER: return ServerHelper::canConvertToInt(param);

		default: return false;
	}
}

Endpoint EndpointManager::getEndpointEntity(QString url, Request::MethodType method)
{
	for (int i = 0; i < instance().endpoint_registry_.count(); ++i)
	{
		if ((instance().endpoint_registry_[i].url.toLower() == url.toLower()) &&
			(instance().endpoint_registry_[i].method == method))
			return instance().endpoint_registry_[i];


		qDebug() << instance().endpoint_registry_[i].url;
	}

	return Endpoint{};
}

QString EndpointManager::getEndpointHelpTemplate(QList<Endpoint>* endpoint_list)
{
	QString output;
	QTextStream stream(&output);

	stream << WebEntity::getPageHeader();
	stream << WebEntity::getApiHelpHeader("API Help Page");

	for (int i = 0; i < endpoint_list->count(); ++i)
	{
		QList<QString> param_names {};
		QList<QString> param_desc {};

		QMapIterator<QString, ParamProps> p((*endpoint_list)[i].params);
		while (p.hasNext()) {
			p.next();
			param_names.append(p.key());
			param_desc.append(p.value().comment);
		}

		stream << WebEntity::getApiHelpEntiry(
					  (*endpoint_list)[i].url,
					  WebEntity::convertMethodTypeToString((*endpoint_list)[i].method),
					  param_names,
					  param_desc,
					  (*endpoint_list)[i].comment
					);
	}

	stream << WebEntity::getPageFooter();

	return output;
}
