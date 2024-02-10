#pragma once

#include "AzCore/JSON/prettywriter.h"
#include "AzCore/Outcome/Outcome.h"
#include "AzCore/std/string/string.h"
#include "Engine/Common_BopAudio.h"
#include "rapidjson/document.h"
#include "rapidjson/schema.h"

namespace BopAudio
{
    static inline auto ValidateDocument(rapidjson::Document const& doc)
        -> AZ::Outcome<void, AZStd::string>
    {
        rapidjson::Document schemaDoc{};

        if (schemaDoc.Parse(DocumentJsonSchema).HasParseError())
        {
            return AZ::Failure("The schema document failed to parse. Check it for errors! "
                               "Validation cannot be completed without a valid schema.");
        }

        rapidjson::SchemaDocument jsonSchema(schemaDoc);
        rapidjson::SchemaValidator validator(jsonSchema);

        if (!doc.Accept(validator))
        {
            rapidjson::StringBuffer error;
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(error);
            validator.GetError().Accept(writer);

            return AZ::Failure(error.GetString());
        }

        return AZ::Success();
    }

} // namespace BopAudio
