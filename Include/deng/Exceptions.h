// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Exceptions.h - DENG exception classes header
// author: Karl-Mihkel Ott

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

namespace DENG {

	class ShaderException : public std::exception {
		private:
			std::string m_szWhatMessage;

		public:
			ShaderException(const std::string& _sWhat) {
				if (_sWhat == "")
					m_szWhatMessage = "Unknown exception";
				else
					m_szWhatMessage = _sWhat;
			}

			const char* what() const override {
				return m_szWhatMessage.c_str();
			}
	};


	class ScriptException : public std::exception {
		private:
			std::string m_szWhatMessage;

		public:
			ScriptException(const std::string& _sWhat) {
				if (_sWhat == "")
					m_szWhatMessage = "Unknown exception";
				else
					m_szWhatMessage = _sWhat;
			}

			const char* what() const override {
				return m_szWhatMessage.c_str();
			}
	};


	class ConventionException : public std::exception {
		private:
			std::string m_szWhatMessage;

		public:
			ConventionException(const std::string& _sWhat) {
				if (_sWhat == "")
					m_szWhatMessage = _sWhat;
				else m_szWhatMessage = _sWhat;
			}

			const char* what() const override {
				return m_szWhatMessage.c_str();
			}
	};


	class IOException : public std::exception {
		private:
			std::string m_szWhatMessage;

		public:
			IOException(const std::string& _sWhat) {
				if (_sWhat == "")
					m_szWhatMessage = "Unknown exception";
				else m_szWhatMessage = _sWhat;
			}

			const char* what() const override {
				return m_szWhatMessage.c_str();
			}
	};


	class EntityException : public std::exception {
		private:
			std::string m_szWhatMessage;

		public:
			EntityException(const std::string& _sWhat) {
				if (_sWhat == "")
					m_szWhatMessage = "Unknown exception";
				else
					m_szWhatMessage = _sWhat;
			}

			const char* what() const override {
				return m_szWhatMessage.c_str();
			}
	};


	class LogicException : public std::exception {
		private:
			std::string m_szWhatMessage;

		public:
			LogicException(const std::string& _sWhat) {
				if (_sWhat == "")
					m_szWhatMessage = "Unknown exception";
				else m_szWhatMessage = _sWhat;
			}

			const char* what() const override {
				return m_szWhatMessage.c_str();
			}
	};


	class HardwareException : public std::exception {
		private:
			std::string m_szWhatMessage;

		public:
			HardwareException(const std::string& _sWhat) {
				if (_sWhat == "")
					m_szWhatMessage = "Unknown exception";
				else m_szWhatMessage = _sWhat;
			}

			const char* what() const override {
				return m_szWhatMessage.c_str();
			}
	};


	class RendererException : public std::exception {
		private:
			std::string m_szWhatMessage;

		public:
			RendererException(const std::string& _sWhat) {
				if (_sWhat == "")
					m_szWhatMessage = "Unknown exception";
				else m_szWhatMessage = _sWhat;
			}

			const char* what() const override {
				return m_szWhatMessage.c_str();
			}
	};


	class SyscallException : public std::exception {
		private:
			std::string m_szWhatMessage;

		public:
			SyscallException(const std::string& _sWhat) {
				if (_sWhat == "")
					m_szWhatMessage = "Unknown exception";
				else m_szWhatMessage = _sWhat;
			}

			const char* what() const override {
				return m_szWhatMessage.c_str();
			}
	};


	class WindowContextException : public std::exception {
		private:
			std::string m_szWhatMessage;

		public:
			WindowContextException(const std::string& _sWhat) {
				if (_sWhat == "")
					m_szWhatMessage = "Unknown exception";
				else m_szWhatMessage = _sWhat;
			}

			const char* what() const override {
				return m_szWhatMessage.c_str();
			}
	};
}

#endif
