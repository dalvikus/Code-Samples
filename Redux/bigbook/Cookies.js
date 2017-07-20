export function saveCookie(key, value)
{
////console.log('saveBookmarks');
    let d = new Date()
    const oneYearInMilliseconds = 365 * 24 * 60 * 60 * 1000;
    d.setTime(d.getTime() + oneYearInMilliseconds);
    let strExpires = "expires=" + d.toGMTString();
    document.cookie = key + '=' + encodeURIComponent(value) + ";" + strExpires + ";path=/";
}

export function deleteCookie(key)
{
////console.log('deleteCookie');
    if (key === '') {
        console.error('No key');
    } else {
////    console.log('  key: "' + key + '"');
        const strExpiresInPast = 'expires=Thu, 01 Jan 1970 00:00:01 GMT';
        document.cookie = key + '=;' + strExpiresInPast + ";path=/";
    }
}

export function listAllCookies()
{
////console.log('listAllCookies');
    let cookies = document.cookie;
    if (cookies === '') {
        console.log('No cookie(s)');
        return;
    }

    cookies.split('; ').forEach((keyEqualValue) => {
////    console.log('  keyEqualValue: "' + keyEqualValue + '"')
        let keyValuePair = keyEqualValue.split('=');
        console.log('  key: "' + keyValuePair[0] + '", value: "' + keyValuePair[1] + '"');
    });
}

export function deleteAllCookies()
{
////console.log('deleteAllCookies');
    let cookies = document.cookie;
    if (cookies === '') {
        console.log('No cookie(s)');
        return;
    }

    cookies.split('; ').forEach((keyEqualValue) => {
////    console.log('  keyEqualValue: "' + keyEqualValue + '"')
        let keyValuePair = keyEqualValue.split('=');
////    console.log('  key: "' + keyValuePair[0] + '", value: "' + keyValuePair[1] + '"');
        deleteCookie(keyValuePair[0]);
    });
}

export function loadCookie(key)
{
    let cookies = document.cookie;
    if (cookies === '') {
        console.log('No cookie(s)');
        return '';
    }

    var value = '';
    cookies.split('; ').forEach((keyEqualValue) => {
////    console.log('keyEqualValue: "' + keyEqualValue + '"')
        let keyValuePair = keyEqualValue.split('=');
////    console.log('key: "' + keyValuePair[0] + '", value: "' + keyValuePair[1] + '"');
        if (keyValuePair[0] === key)
            value = keyValuePair[1];
    });
    if (value === '') {
        console.error('No cookie for "' + key + '"');
        return '';
    }
    return decodeURIComponent(value)
}
