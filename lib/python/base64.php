<?php
namespace python;

use \PyModule;
use \PyCore;

/**
Base16, Base32, Base64 (RFC 3548), Base85 and Ascii85 data encodings*/
class base64{
    private static ?PyModule $__module = null;

    public static function __init(): void {
        if (self::$__module == null) {
            self::$__module = PyCore::import('base64');
            self::$_b32rev = self::$__module->_b32rev;
            self::$_b32tab2 = self::$__module->_b32tab2;
            self::$binascii = self::$__module->binascii;
            self::$bytes_types = self::$__module->bytes_types;
            self::$re = self::$__module->re;
            self::$struct = self::$__module->struct;
        }
    }

    public const MAXBINSIZE = 57;
    public const MAXLINESIZE = 76;

    public static $_A85END = "~>";
    public static $_A85START = "<~";
    public static $_B32_DECODE_DOCSTRING = "\nDecode the {encoding} encoded bytes-like object or ASCII string s.\n\nOptional casefold is a flag specifying whether a lowercase alphabet is\nacceptable as input.  For security purposes, the default is False.\n{extra_args}\nThe result is returned as a bytes object.  A binascii.Error is raised if\nthe input is incorrectly padded or if there are non-alphabet\ncharacters present in the input.\n";
    public static $_B32_DECODE_MAP01_DOCSTRING = "\nRFC 3548 allows for optional mapping of the digit 0 (zero) to the\nletter O (oh), and for optional mapping of the digit 1 (one) to\neither the letter I (eye) or letter L (el).  The optional argument\nmap01 when not None, specifies which letter the digit 1 should be\nmapped to (when map01 is not None, the digit 0 is always mapped to\nthe letter O).  For security purposes the default is None, so that\n0 and 1 are not allowed in the input.\n";
    public static $_B32_ENCODE_DOCSTRING = "\nEncode the bytes-like objects using {encoding} and return a bytes object.\n";
    public static $__name__ = "base64";
    public static $__package__ = "";
    public static $_a85chars = null;
    public static $_a85chars2 = null;
    public static $_b32alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    public static $_b32hexalphabet = "0123456789ABCDEFGHIJKLMNOPQRSTUV";
    public static $_b85alphabet = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~";
    public static $_b85chars = null;
    public static $_b85chars2 = null;
    public static $_b85dec = null;
    public static $_urlsafe_decode_translation = " 	\n !\"#$%&'()*+,+./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^/`abcdefghijklmnopqrstuvwxyz{|}~��������������������������������������������������������������������������������������������������������������������������������";
    public static $_urlsafe_encode_translation = " 	\n !\"#$%&'()*-,-._0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~��������������������������������������������������������������������������������������������������������������������������������";

    public static $_b32rev = null;
    public static $_b32tab2 = null;
    public static $binascii = null;
    public static $bytes_types = null;
    public static $re = null;
    public static $struct = null;

    public static function _85encode($b, $chars, $chars2, $pad=false, $foldnuls=false, $foldspaces=false)
    {
        return self::$__module->_85encode($b, $chars, $chars2, $pad, $foldnuls, $foldspaces);
    }
    public static function _b32decode($alphabet, $s, $casefold=false, $map01=null)
    {
        return self::$__module->_b32decode($alphabet, $s, $casefold, $map01);
    }
    public static function _b32encode($alphabet, $s)
    {
        return self::$__module->_b32encode($alphabet, $s);
    }
    public static function _bytes_from_decode_data($s)
    {
        return self::$__module->_bytes_from_decode_data($s);
    }
    public static function _input_type_check($s)
    {
        return self::$__module->_input_type_check($s);
    }
    public static function a85decode($b)
    {
        return self::$__module->a85decode($b);
    }
    public static function a85encode($b)
    {
        return self::$__module->a85encode($b);
    }
    public static function b16decode($s, $casefold=false)
    {
        return self::$__module->b16decode($s, $casefold);
    }
    public static function b16encode($s)
    {
        return self::$__module->b16encode($s);
    }
    public static function b32decode($s, $casefold=false, $map01=null)
    {
        return self::$__module->b32decode($s, $casefold, $map01);
    }
    public static function b32encode($s)
    {
        return self::$__module->b32encode($s);
    }
    public static function b32hexdecode($s, $casefold=false)
    {
        return self::$__module->b32hexdecode($s, $casefold);
    }
    public static function b32hexencode($s)
    {
        return self::$__module->b32hexencode($s);
    }
    public static function b64decode($s, $altchars=null, $validate=false)
    {
        return self::$__module->b64decode($s, $altchars, $validate);
    }
    public static function b64encode($s, $altchars=null)
    {
        return self::$__module->b64encode($s, $altchars);
    }
    public static function b85decode($b)
    {
        return self::$__module->b85decode($b);
    }
    public static function b85encode($b, $pad=false)
    {
        return self::$__module->b85encode($b, $pad);
    }
    public static function decode($input, $output)
    {
        return self::$__module->decode($input, $output);
    }
    public static function decodebytes($s)
    {
        return self::$__module->decodebytes($s);
    }
    public static function encode($input, $output)
    {
        return self::$__module->encode($input, $output);
    }
    public static function encodebytes($s)
    {
        return self::$__module->encodebytes($s);
    }
    public static function main()
    {
        return self::$__module->main();
    }
    public static function standard_b64decode($s)
    {
        return self::$__module->standard_b64decode($s);
    }
    public static function standard_b64encode($s)
    {
        return self::$__module->standard_b64encode($s);
    }
    public static function test()
    {
        return self::$__module->test();
    }
    public static function urlsafe_b64decode($s)
    {
        return self::$__module->urlsafe_b64decode($s);
    }
    public static function urlsafe_b64encode($s)
    {
        return self::$__module->urlsafe_b64encode($s);
    }
}

base64::__init();
