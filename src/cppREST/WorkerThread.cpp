#include "WorkerThread.h"

WorkerThread::WorkerThread(Request request)
	: request_(request)
{
}

void WorkerThread::run()
{
	qDebug() << "Processing path:" << request_.path;

		QByteArray body {};
		qInfo().noquote() << WebEntity::convertMethodTypeToString(request_.method).toUpper() << "/" << request_.path;

		Endpoint current_endpoint = EndpointManager::getEndpointEntity(request_.path, request_.method);
		if (current_endpoint.action_func == nullptr)
		{
			emit resultReady(WebEntity::createError(ErrorType::BAD_REQUEST, request_.return_type, "This action cannot be processed"));
			return;
		}

		try
		{
			EndpointManager::validateInputData(&current_endpoint, request_);
		}
		catch (ArgumentException& e)
		{
			emit resultReady(WebEntity::createError(ErrorType::BAD_REQUEST, request_.return_type, e.message()));
			return;
		}

		endpoint_action_ = current_endpoint.action_func;
		Response response = (*endpoint_action_)(request_);
		if (!response.body.isNull())
		{
			emit resultReady(response);
			return;
		}

		emit resultReady(WebEntity::createError(ErrorType::NOT_FOUND, request_.return_type, "This page does not exist. Check the URL and try again"));
}
