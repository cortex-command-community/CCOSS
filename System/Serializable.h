#ifndef _RTESERIALIZEABLE_
#define _RTESERIALIZEABLE_

#include "Reader.h"
#include "Writer.h"

#include <functional>
#include <string>
#include <unordered_map>

namespace RTE {

	/// <summary>
	/// This base class specifies common creation/destruction patterns associated with reading and writing member data from disk.
	/// Is only intended to be inherited from in one level.
	/// </summary>
	class Serializable {

	public:

#pragma region Global Macro Definitions
		/// <summary>
		/// Convenience macro to cut down on duplicate ReadProperty and Save methods in classes that extend Serializable.
		/// </summary>
		#define SerializableOverrideMethods \
			int ReadProperty(const std::string_view &propName, Reader &reader) override; \
			int Save(Writer &writer) const override;

		/// <summary>
		/// Convenience macro to cut down on duplicate GetClassName methods in non-poolable classes that extend Serializable.
		/// </summary>
		#define SerializableClassNameGetter \
			const std::string & GetClassName() const override { return c_ClassName; }

		#define StartPropertyList(failureCase) \
			static std::unordered_map<std::string, int> sm_Properties; \
			static bool sm_Initialized = false; \
			int jmpAddress = sm_Initialized ? -1 : -2; \
			if (sm_Initialized) { \
				auto itr = sm_Properties.find(std::string(propName)); \
				if (itr == sm_Properties.end()) { \
					failureCase; \
				} else { \
					jmpAddress = itr->second; \
				} \
			} \
			switch (jmpAddress) { \
				case -1: break; \
				case -2:

			#define MatchForwards(propertyName) \
				sm_Properties[propertyName] = __COUNTER__ + 1; \
				case __COUNTER__:

			#define MatchProperty(propertyName,matchedFunction) \
				sm_Properties[propertyName] = __COUNTER__ + 1; \
				case __COUNTER__: if (sm_Initialized) { { matchedFunction }; break; };

		#define EndPropertyList } \
			if (!sm_Initialized) { \
				sm_Initialized = true; \
				/* This was the initialization pass, now properly read the value by calling ourselves again */ \
				return ReadProperty(propName, reader); \
			} \
			return 0;

#pragma endregion

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a Serializable object in system memory. Create() should be called before using the object.
		/// </summary>
		Serializable() = default;

		/// <summary>
		/// Makes the Serializable object ready for use, usually after all necessary properties have been set with Create(Reader).
		/// </summary>
		/// <returns>An error return value signaling success or any particular failure. Anything below 0 is an error signal.</returns>
		virtual int Create() { return 0; }

		/// <summary>
		/// Makes the Serializable object ready for use.
		/// </summary>
		/// <param name="reader">A Reader that the Serializable will create itself from.</param>
		/// <param name="checkType">Whether there is a class name in the stream to check against to make sure the correct type is being read from the stream.</param>
		/// <param name="doCreate">Whether to do any additional initialization of the object after reading in all the properties from the Reader. This is done by calling Create().</param>
		/// <param name="skipStartingObject>Skips notifying the Reader that a new object is being read in order to associate any subsequent reads with the currently read object. Special edge case for DataModules that use ScanFolderContents.</param>
		/// <returns>An error return value signaling success or any particular failure. Anything below 0 is an error signal.</returns>
		int CreateSerializable(Reader &reader, bool checkType, bool doCreate, bool skipStartingObject);

		/// <summary>
		/// Makes the Serializable object ready for use.
		/// </summary>
		/// <param name="reader">A Reader that the Serializable will create itself from.</param>
		/// <param name="checkType">Whether there is a class name in the stream to check against to make sure the correct type is being read from the stream.</param>
		/// <param name="doCreate">Whether to do any additional initialization of the object after reading in all the properties from the Reader. This is done by calling Create().</param>
		/// <returns>An error return value signaling success or any particular failure. Anything below 0 is an error signal.</returns>
		virtual int Create(Reader &reader, bool checkType = true, bool doCreate = true) { return CreateSerializable(reader, checkType, doCreate, false); }
#pragma endregion

#pragma region Destruction
		/// <summary>
		/// Destructor method used to clean up a Serializable object before deletion from system memory.
		/// </summary>
		virtual ~Serializable() = default;

		/// <summary>
		/// Resets the entire Serializable, including its inherited members, to their default settings or values.
		/// </summary>
		virtual void Reset() { Clear(); }
#pragma endregion

#pragma region INI Handling
		/// <summary>
		/// Reads a property value from a Reader stream.
		/// If the name isn't recognized by this class, then ReadProperty of the parent class is called.
		/// If the property isn't recognized by any of the base classes, false is returned, and the Reader's position is untouched.
		/// </summary>
		/// <param name="propName">The name of the property to be read.</param>
		/// <param name="reader">A Reader lined up to the value of the property to be read.</param>
		/// <returns>
		/// An error return value signaling whether the property was successfully read or not.
		/// 0 means it was read successfully, and any nonzero indicates that a property of that name could not be found in this or base classes.
		/// </returns>
		virtual int ReadProperty(const std::string_view &propName, Reader &reader);

		/// <summary>
		/// Saves the complete state of this Serializable to an output stream for later recreation with Create(istream &stream).
		/// </summary>
		/// <param name="writer">A Writer that the Serializable will save itself to.</param>
		/// <returns>An error return value signaling success or any particular failure. Anything below 0 is an error signal.</returns>
		virtual int Save(Writer &writer) const { writer.ObjectStart(GetClassName()); return 0; }

		/// <summary>
		/// Replaces backslashes with forward slashes in file paths to eliminate issues with cross-platform compatibility or invalid escape sequences.
		/// </summary>
		/// <param name="pathToCorrect">Reference to the file path string to correct slashes in.</param>
		// TODO: Add a warning log entry if backslashes are found in a data path. Perhaps overwrite them in the ini file itself.
		std::string CorrectBackslashesInPath(const std::string &pathToCorrect) const { return std::filesystem::path(pathToCorrect).generic_string(); }
#pragma endregion

#pragma region Logging
		/// <summary>
		/// Sets the file and line that are currently being read. Formatted to be used for logging warnings and errors.
		/// Because we're often used as a parent for basic types (i.e, Vector, Matrix, Color), where we don't want to spend any time doing string construction we don't actually store this data here.
		/// This just acts as an abstract base for child classes to implement.
		/// </summary>
		/// <param name="newPosition">A string containing the currently read file path and the line being read.</returns>
		virtual void SetFormattedReaderPosition(const std::string &newPosition) {}
#pragma endregion

#pragma region Operator Overloads
		/// <summary>
		/// A Reader extraction operator for filling a Serializable from a Reader.
		/// </summary>
		/// <param name="reader">A Reader reference as the left hand side operand.</param>
		/// <param name="operand">An Serializable reference as the right hand side operand.</param>
		/// <returns>A Reader reference for further use in an expression.</returns>
		friend Reader & operator>>(Reader &reader, Serializable &operand);

		/// <summary>
		/// A Reader extraction operator for filling an Serializable from a Reader.
		/// </summary>
		/// <param name="reader">A Reader reference as the left hand side operand.</param>
		/// <param name="operand">An Serializable pointer as the right hand side operand.</param>
		/// <returns>A Reader reference for further use in an expression.</returns>
		friend Reader & operator>>(Reader &reader, Serializable *operand);

		/// <summary>
		/// A Writer insertion operator for sending a Serializable to a Writer.
		/// </summary>
		/// <param name="writer">A Writer reference as the left hand side operand.</param>
		/// <param name="operand">A Serializable reference as the right hand side operand.</param>
		/// <returns>A Writer reference for further use in an expression.</returns>
		friend Writer & operator<<(Writer &writer, const Serializable &operand);

		/// <summary>
		/// A Writer insertion operator for sending a Serializable to a Writer.
		/// </summary>
		/// <param name="writer">A Writer reference as the left hand side operand.</param>
		/// <param name="operand">A Serializable pointer as the right hand side operand.</param>
		/// <returns>A Writer reference for further use in an expression.</returns>
		friend Writer &operator<<(Writer &writer, const Serializable *operand);
#pragma endregion

#pragma region Class Info
		/// <summary>
		/// Gets the class name of this Serializable.
		/// </summary>
		/// <returns>A string with the friendly-formatted type name of this Serializable.</returns>
		virtual const std::string & GetClassName() const = 0;
#pragma endregion

	private:

		/// <summary>
		/// Clears all the member variables of this Object, effectively resetting the members of this abstraction level only.
		/// </summary>
		void Clear() {}
	};
}
#endif