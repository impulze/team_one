#include "Document.h"

#include <stdexcept>

/**
 * @file Document.cpp
 * @author Daniel Mierswa <daniel.mierswa@student.hs-rm.de>
 *
 * Implementation file for the document implementation.
 */

Document::Document(std::string const &, bool)
{
	throw std::runtime_error("STUB Document::Document");
}

void Document::remove()
{
	throw std::runtime_error("STUB Document::remove");
}

void Document::save()
{
	throw std::runtime_error("STUB Document::save");
}

std::vector<std::string> Document::list_documents()
{
	throw std::runtime_error("STUB Document::list_documents");
}
