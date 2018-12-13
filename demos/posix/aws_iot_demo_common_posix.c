/*
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * @file aws_iot_mqtt_demo_common_posix.c
 * @brief Implements the common demo functions for POSIX systems.
 */

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Common demo includes. */
#include "aws_iot_demo.h"
#include "aws_iot_demo_posix.h"

/*-----------------------------------------------------------*/

bool AwsIotDemo_ParseArguments( int argc,
                                char ** argv,
                                AwsIotDemoArguments_t * const pArguments )
{
    int option = 0;
    unsigned long int port = 0;

    /* Default to AWS IoT MQTT mode. */
    pArguments->awsIotMqttMode = true;

    /* Set default secured connection status if defined. */
    #ifdef AWS_IOT_DEMO_SECURED_CONNECTION
        pArguments->securedConnection = AWS_IOT_DEMO_SECURED_CONNECTION;
    #endif

    /* Set default MQTT server if defined. */
    #ifdef AWS_IOT_DEMO_SERVER
        pArguments->pHostName = AWS_IOT_DEMO_SERVER;
    #endif

    /* Set default MQTT server port if defined. */
    #ifdef AWS_IOT_DEMO_PORT
        pArguments->port = AWS_IOT_DEMO_PORT;
    #endif

    /* Set default root CA path if defined. */
    #ifdef AWS_IOT_DEMO_ROOT_CA
        pArguments->pRootCaPath = AWS_IOT_DEMO_ROOT_CA;
    #endif

    /* Set default client certificate path if defined. */
    #ifdef AWS_IOT_DEMO_CLIENT_CERT
        pArguments->pClientCertPath = AWS_IOT_DEMO_CLIENT_CERT;
    #endif

    /* Set default client certificate private key path if defined. */
    #ifdef AWS_IOT_DEMO_PRIVATE_KEY
        pArguments->pPrivateKeyPath = AWS_IOT_DEMO_PRIVATE_KEY;
    #endif

    AwsIotLogInfo( "Parsing command line arguments." );

    /* Silence any error or warning messages printed by the system. The demos
     * will use the logging library instead. */
    opterr = 0;

    /* Retrieve all command line arguments. */
    while( ( option = getopt( argc, argv, ":sunh:p:r:c:k:i:" ) ) != -1 )
    {
        switch( option )
        {
            /* Secured connection. */
            case ( int ) ( 's' ):
                pArguments->securedConnection = true;

                break;

            /* Unsecured connection. */
            case ( int ) ( 'u' ):
                pArguments->securedConnection = false;

                break;

            /* MQTT server is not AWS IoT. */
            case ( int ) ( 'n' ):
                pArguments->awsIotMqttMode = false;
                break;

            /* Server. */
            case ( int ) ( 'h' ):
                pArguments->pHostName = optarg;
                break;

            /* Server port. */
            case ( int ) ( 'p' ):
                /* Convert argument string to unsigned int. */
                port = strtoul( optarg, NULL, 10 );

                /* Check that port is valid. */
                if( ( port == 0 ) || ( port > UINT16_MAX ) )
                {
                    AwsIotLogWarn( "Ignoring invalid port '%lu'.", port );
                }
                else
                {
                    pArguments->port = ( uint16_t ) port;
                }

                break;

            /* Root CA path. */
            case ( int ) ( 'r' ):
                pArguments->pRootCaPath = optarg;
                break;

            /* Client certificate path. */
            case ( int ) ( 'c' ):
                pArguments->pClientCertPath = optarg;
                break;

            /* Client certificate private key path. */
            case ( int ) ( 'k' ):
                pArguments->pPrivateKeyPath = optarg;
                break;

            /* Client identifier or Thing Name. */
            case ( int ) ( 'i' ):
                pArguments->pIdentifier = optarg;
                break;

            /* Unknown argument. */
            case ( int ) ( '?' ):
                AwsIotLogWarn( "Ignoring unknown argument '-%c'.", ( char ) optopt );
                break;

            /* Argument known, but missing value. */
            case ( int ) ( ':' ):
                AwsIotLogWarn( "Ignoring invalid argument '-%c'. Option '-%c' requires a value.",
                               ( char ) optopt,
                               ( char ) optopt );
                break;

            /* The default case should not be executed. */
            default:
                abort();
                break;
        }
    }

    /* Check that a server was set. */
    if( ( pArguments->pHostName == NULL ) ||
        ( strlen( pArguments->pHostName ) == 0 ) )
    {
        AwsIotLogError( "MQTT server not set. Exiting." );

        return false;
    }

    /* Check that a server port was set. */
    if( pArguments->port == 0 )
    {
        AwsIotLogError( "MQTT server port not set. Exiting." );

        return false;
    }

    /* Check credentials for a secured connection. */
    if( pArguments->securedConnection == true )
    {
        /* Check that a root CA path was set. */
        if( ( pArguments->pRootCaPath == NULL ) ||
            ( strlen( pArguments->pRootCaPath ) == 0 ) )
        {
            AwsIotLogError( "Root CA path not set. Exiting." );

            return false;
        }

        /* Check that a client certificate path was set. */
        if( ( pArguments->pClientCertPath == NULL ) ||
            ( strlen( pArguments->pClientCertPath ) == 0 ) )
        {
            AwsIotLogError( "Client certificate path not set. Exiting." );

            return false;
        }

        /* Check that a client certificate private key was set. */
        if( ( pArguments->pPrivateKeyPath == NULL ) ||
            ( strlen( pArguments->pPrivateKeyPath ) == 0 ) )
        {
            AwsIotLogError( "Client certificate private key not set. Exiting." );

            return false;
        }
    }

    AwsIotLogInfo( "Command line arguments successfully parsed." );

    AwsIotLogDebug( "AWS IoT MQTT mode: %s", pArguments->awsIotMqttMode == true ? "true" : "false" );
    AwsIotLogDebug( "Secured connection: %s", pArguments->securedConnection == true ? "true" : "false" );
    AwsIotLogDebug( "Host: %s", pArguments->pHostName );
    AwsIotLogDebug( "Port: %hu", pArguments->port );
    AwsIotLogDebug( "Root CA: %s", pArguments->pRootCaPath );
    AwsIotLogDebug( "Client certificate: %s", pArguments->pClientCertPath );
    AwsIotLogDebug( "Private key: %s", pArguments->pPrivateKeyPath );

    return true;
}

/*-----------------------------------------------------------*/