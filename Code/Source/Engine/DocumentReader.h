#pragma once

#include <AzCore/IO/Path/Path.h>
#include <AzCore/JSON/prettywriter.h>
#include <AzCore/JSON/schema.h>

#include "AzCore/IO/FileIO.h"
#include "AzCore/Outcome/Outcome.h"
#include "AzCore/std/containers/vector.h"
#include "AzCore/std/smart_ptr/make_shared.h"
#include "AzCore/std/string/string.h"

#include "Engine/Common_BopAudio.h"

namespace BopAudio
{
    class AudioDocument
    {
    public:
        AudioDocument() = default;
        AudioDocument(AZ::IO::Path documentPath)
            : m_docPath{ AZStd::move(documentPath) }
        {
        }

        auto Load() -> AZ::Outcome<void, char const*>
        {
            AZ::IO::FileIOStream fileStream{ m_docPath.c_str(), AZ::IO::OpenMode::ModeRead };
            m_buffer.reserve(fileStream.GetLength());
            fileStream.Read(m_buffer.size(), m_buffer.data());

            return AZ::Success();
        }

        [[nodiscard]] auto GetJsonDoc() const -> rapidjson::Document const*
        {
            return m_jsonDoc.get();
        }

    private:
        AZ::IO::Path m_docPath{};
        AZStd::vector<char> m_buffer{};
        AZStd::shared_ptr<rapidjson::Document> m_jsonDoc{};
    };

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

    static inline auto LoadDocument(AZ::IO::Path documentPath)
        -> AZStd::shared_ptr<rapidjson::Document>
    {
        auto doc = AZStd::make_shared<rapidjson::Document>();

        return doc;
    }

} // namespace BopAudio
