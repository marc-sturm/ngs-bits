#include "ProjectModeSwitcher.h"

ProjectModeSwitcher::ProjectModeSwitcher()
{
}

ProjectModeSwitcher::~ProjectModeSwitcher()
{
}

ProjectModeSwitcher& ProjectModeSwitcher::instance()
{
	static ProjectModeSwitcher instance;
	return instance;
}

void ProjectModeSwitcher::setIsLocalProject(QSharedPointer<bool> is_local_project)
{
	qDebug() << "Instance" << *is_local_project;
	instance().is_local_project_ = is_local_project;
}

bool ProjectModeSwitcher::isLocalProject()
{
	if (instance().is_local_project_.isNull())
	{
		THROW(ProgrammingException, "Project mode switcher requested but not set!");
	}

	return *instance().is_local_project_.data();
}

void ProjectModeSwitcher::setProjectFileName(QSharedPointer<QString> project_file_name)
{
	instance().project_file_name_ = project_file_name;
}

const QSharedPointer<QString> ProjectModeSwitcher::projectFileName()
{
	if (instance().project_file_name_.isNull())
	{
		THROW(ProgrammingException, "Project mode switcher requested but not set!");
	}

	return instance().project_file_name_;
}
