package de.teamone.cte;

@SuppressWarnings("serial")
public class CTEException extends Exception
{
	enum ExceptionType
	{
		UNKNOWN_EXCEPTION,
		CONNECTION_FAILED,
		INVALID_TYPE,
		UNEXPECTED_EXCEPTION
	}
	
	public final ExceptionType type;
	
	/**
	 * Standard constructor.
	 * @param message
	 * @param type
	 * @param cause
	 */
	public CTEException(String message, ExceptionType type, Throwable cause)
	{
		super(message, cause);
		this.type = type;
	}
	
	/**
	 * Simplified standard constructor.
	 * cause defaulted to `null`.
	 * @param message
	 * @param type
	 */
	public CTEException(String message, ExceptionType type)
	{ this(message, type, null); }
}
