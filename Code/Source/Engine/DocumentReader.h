#pragma once

#include "AzCore/Outcome/Outcome.h"
#include "AzCore/std/containers/span.h"
#include "Engine/Common_BopAudio.h"
#include "rapidjson/document.h"
#include "rapidjson/schema.h"

namespace BopAudio
{
    inline auto ValidateDocument(rapidjson::Document const& doc) -> AZ::Outcome<void, char const*>
    {
        rapidjson::Document schemaDoc{};

        if (schemaDoc.Parse(DocumentJsonSchema).HasParseError())
        {
            return AZ::Failure("The schema document failed to parse. Check it for errors! "
                               "Validation cannot be completed without a valid schema.");
        }

        rapidjson::SchemaDocument jsonSchema(schemaDoc);
        rapidjson::SchemaValidator validator(jsonSchema);

        doc.Accept(validator);

        return AZ::Success();
    }

    inline auto LoadDocument(AZStd::span<char const> buffer) -> rapidjson::Document&&
    {
        rapidjson::Document doc{};
        doc.Parse(buffer.data(), buffer.size());

        return AZStd::move(doc);
    }
} // namespace BopAudio
